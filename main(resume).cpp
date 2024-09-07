#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <memory> // For smart pointers
#include <map> // For associative containers
#include <string>
#include <sqlite3.h>   // For SQLite
#include <sodium.h>    // For Sodium
#include <SDL_ttf.h>   // For SDL_ttf


#include <ctime> // For handling time-related functions
#include <regex> // For regular expressions
#include <cmath>

#include <fstream> // For file input and output
#include <chrono> // For time manipulation
#include <stdexcept> // For standard exceptions
#include <utility> // For utility functions
#include <deque> // For double-ended queues

#include <cassert> // For assertions
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace std;

// Forward declaration of all of the base classes
class State;
class User;
class NavigationMenu;
// Enumeration of states
enum AppState {
    SPLASH_SCREEN,
    INTRO_SCREEN,
    MAIN_DASHBOARD,
      PROFILE_SCREEN,
    EDUCATION_SCREEN,
    PROJECTS_SCREEN,
    CERTIFICATES_SCREEN,
    TARGETS_SCREEN,
    ELIGIBILITY_REASON_SCREEN,
SOCIALS_SCREEN ,
CONTACT_INFORMATION_SCREEN,
};


// Screen names corresponding to State
const char* screenNames[] = {
    "Splash Screen",
    "Intro Screen",
    "Main Dashboard Screen",
 "Profile Screen",
"Education Screen",
"Projects Scren",
"Certificates Screen",
"Targets Screen",
"Eligibility Reason Screen",
"Socials Screen",
"Contact Information Screen"


};

// Globals
int Width = 800;
int Height = 600;
int boxWidth = Width - 53;
const int offset=20;  


 int boxPadding;
 SDL_Window *window = nullptr;
 SDL_Renderer *renderer = nullptr;
 TTF_Font *NunitoFont = nullptr;
 TTF_Font *BrilliantDreamsFont = nullptr;
 TTF_Font *BrockScript = nullptr;
 TTF_Font *smallNimbusRomFont = nullptr;
 TTF_Font *NimbusRomFont= nullptr;
 TTF_Font * middlesizeNimbusRomFont= nullptr;
 std::unique_ptr<User> currentUser = nullptr;
 AppState currentState = SPLASH_SCREEN;
 std::unique_ptr<State> currentStateInstance;
 Uint32 startTime = 0;

 void changeState(AppState newState);

 SDL_Texture *loadTexture(const std::string &path, SDL_Renderer *renderer)
 {
     SDL_Texture *newTexture = nullptr;
     SDL_Surface *loadedSurface = IMG_Load(path.c_str());
     if (loadedSurface == nullptr)
     {
         std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
     }
     else
     {
         newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
         if (newTexture == nullptr)
         {
             std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
         }
         SDL_FreeSurface(loadedSurface);
     }
     return newTexture;
}

const int radius = 15;
void renderRoundedRect(SDL_Renderer* renderer, int x, int y, int w, int h, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    // Draw the four corners as circles
    for (int i = 0; i <= radius; i++) {
        for (int j = 0; j <= radius; j++) {
            if (i * i + j * j <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x + radius - i, y + radius - j);
                SDL_RenderDrawPoint(renderer, x + w - radius + i, y + radius - j);
                SDL_RenderDrawPoint(renderer, x + radius - i, y + h - radius + j);
                SDL_RenderDrawPoint(renderer, x + w - radius + i, y + h - radius + j);
            }
        }
    }

    // Draw the sides as rectangles
    SDL_Rect top = { x + radius, y, w - 2 * radius, radius };
    SDL_Rect bottom = { x + radius, y + h - radius, w - 2 * radius, radius };
    SDL_Rect left = { x, y + radius, radius, h - 2 * radius };
    SDL_Rect right = { x + w - radius, y + radius, radius, h - 2 * radius };
    SDL_Rect center = { x + radius, y + radius, w - 2 * radius, h - 2 * radius };

    SDL_RenderFillRect(renderer, &top);
    SDL_RenderFillRect(renderer, &bottom);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);
    SDL_RenderFillRect(renderer, &center);
}
// Function to draw a circle
void DrawCircle(SDL_Renderer* renderer, int centerX, int centerY, int circleradius) {
    int x = circleradius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        // Draw the points in all octants
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y); // Octant 1
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x); // Octant 2
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x); // Octant 3
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y); // Octant 4
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y); // Octant 5
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x); // Octant 6
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x); // Octant 7
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y); // Octant 8

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }

        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}


class User {
private:
    std::string CompanyName;
    std::string username;
    std::string JobTitle;

public:
    User(const std::string& userCompanyname, const std::string& userName, const std::string& pass)
        : CompanyName(userCompanyname), username(userName), JobTitle(JobTitle) {}

    // Getters and setters as needed
    std::string getCompanyName() const { return CompanyName; }
    std::string getUsername() const { return username; }
    std::string  getJobTitle() const { return JobTitle; }
};

//gloabal variables
    
sqlite3* db = nullptr;


bool initializeDatabase(const std::string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Create users table
    const char* createUsersTableSQL = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            CompanyName TEXT NOT NULL UNIQUE,
            username TEXT NOT NULL,
            JobTitle TEXT NOT NULL
        );
    )";

    char* errorMessage = nullptr;
    if (sqlite3_exec(db, createUsersTableSQL, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        std::cerr << "SQL error creating users table: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }

   
    return true;
}















bool insertUser(const User &user);
void setCurrentUser(const std::string &CompanyName, const std::string &username, const std::string &JobTitle);



void CreateUser(const std::string& username, const std::string& CompanyName, const std::string& JobTitle) {
    // Validate user data (this step is simplified; in real applications, you should perform thorough validation)
    if (username.empty() || CompanyName.empty() || JobTitle.empty()) {
        std::cerr << "Error: All fields are required." << std::endl;
        return;
    }


    // Create a User object
    User newUser(CompanyName, username, JobTitle);

    // Save the new user to the database
    if (!insertUser(newUser)) {
        std::cerr << "Error: Failed to save user to database." << std::endl;
    }else {
        std::cout << "User created successfully." << std::endl;
   // Set the current user
        setCurrentUser(CompanyName, username, JobTitle);
    }
}
bool insertUser(const User& user) {
    const char* insertSQL = "INSERT INTO users (CompanyName, username, JobTitle) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    if (sqlite3_bind_text(stmt, 1, user.getCompanyName().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 2, user.getUsername().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, user.getJobTitle().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        std::cerr << "Failed to bind parameters: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

void setCurrentUser(const std::string& CompanyName, const std::string& username, const std::string& JobTitle) {
    currentUser = std::make_unique<User>(CompanyName, username, JobTitle);
}

// Function to render text
void renderText(const std::string& text, int x, int y, SDL_Color color, const std::string& fontPath, int fontSize, SDL_Renderer* renderer) {
    // Load the font with the specified size
    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (font == nullptr) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    // Render the text to a surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (textSurface == nullptr) {
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        TTF_CloseFont(font);
        return;
    }

    // Create a texture from the surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == nullptr) {
        std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        return;
    }

    // Get the dimensions of the text
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    // Define the destination rectangle for rendering
    SDL_Rect textRect = { x, y, textWidth, textHeight };

    // Copy the texture to the renderer
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    // Clean up
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}

std::vector<std::string> wrapText(const std::string& text, TTF_Font* font, int maxWidth) {
    std::vector<std::string> lines;
    std::istringstream paragraphStream(text);
    std::string line;

    while (std::getline(paragraphStream, line)) {
        if (line.empty()) {
            lines.push_back(""); // Add an empty line for '\n\n'
            continue;
        }

        std::istringstream wordsStream(line);
        std::string word;
        std::string wrappedLine;

        while (wordsStream >> word) {
            std::string testLine = wrappedLine.empty() ? word : wrappedLine + " " + word;
            int textWidth;
            TTF_SizeText(font, testLine.c_str(), &textWidth, nullptr);

            if (textWidth > maxWidth) {
                lines.push_back(wrappedLine); // Add the current line to the list
                wrappedLine = word;           // Start a new line with the current word
            } else {
                wrappedLine = testLine; // Continue adding words to the current line
            }
        }

        if (!wrappedLine.empty()) {
            lines.push_back(wrappedLine); // Add the last line of the current segment
        }
    }

    return lines;
}

// Function to render a wrapped and centered paragraph
void renderParagraph(const std::string& paragraph, int startY, SDL_Color color, const std::string& fontPath, int fontSize, SDL_Renderer* renderer, int screenWidth) {
    // Load the font
    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    // Wrap the text to fit within the screen width (with some margin)
    std::vector<std::string> lines = wrapText(paragraph, font, screenWidth - 200); // 200 pixels margin (100 on each side)

    // Calculate the height of a single line of text
    int lineHeight = TTF_FontHeight(font);

    // Render each line centered on the screen
    int y = startY;
    for (const std::string& line : lines) {
        int textWidth;
        TTF_SizeText(font, line.c_str(), &textWidth, nullptr);



        
        int x = (screenWidth - textWidth) / 2; // Center the line horizontally

        // Render the text
        
        renderText(line, x, y, color, fontPath, fontSize, renderer);
        y += lineHeight + 5; // Move to the next line with some spacing
    }

    TTF_CloseFont(font);
}


// Abstract class State
class State {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* NimbusRomFont;
    

    State(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* NimbusRomFont)
        : window(window), renderer(renderer), NimbusRomFont(NimbusRomFont) {}
    virtual void handleEvents(SDL_Event& event) = 0;
    virtual void update() = 0;
    virtual void render(){
    };
    virtual void cleanup() = 0;  // Pure virtual function
    virtual ~State() = default;
     
};


// SplashScreenState class
class SplashScreenState : public State {
public:
    SplashScreenState(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* BrilliantDreamsFont):State(window ,renderer, BrilliantDreamsFont ) {
        // Initialization if needed
    }
    
    void handleEvents(SDL_Event& event) override {
        // Handle events specific to Splash Screen
    }
    
    void update() override {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - startTime > 2500) { 
            changeState(INTRO_SCREEN);
        }
    }
    
    void render() override {
        
        
      



        SDL_Color white = { 255, 255, 255, 255 };
// Get the dimensions of the text
    int textWidth, textHeight;
    TTF_SizeText(BrilliantDreamsFont, "RESUME", &textWidth, &textHeight);


const int textX = (Width-textWidth) / 2; // Center horizontally
    const int textY = (Height-textHeight )/ 2; // Center vertically

        renderText("RESUME",  textX, textY, white,"C:/NEW/assets/Brilliant Dreams.ttf",35, renderer);
    }
void cleanup() override {
        // Implement cleanup logic 
    }

};

class NavigationMenu : public State {
protected:
int scrollOffsetY = 0;
int maxScrollOffset = 0;


    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color grey = { 100, 100, 100, 255 };
    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Color darkgreen = { 0, 50, 0, 255 };
    SDL_Color maroon = { 128, 0, 0, 255 };


        int HomeiconWidth= 40;
        int ProfileiconWidth= 40;
        int EDUCATIONiconWidth= 40;
        int PROJECTSIconWidth= 40;
        int CERTIFICATESIconWidth= 40;
        int TargetsiconWidth= 40;
        int Why_meIconWidth= 40;
        int SOCIALSIconWidth= 40;
        int CONTACTINFORMATIONIconWidth= 40;

        
        int textWidthHome ,textWidthProfile ,textWidthEDUCATION ,textWidthProjects ,textWidthCertificates ,textWidthTargets,textWidthWhy_me,textWidthSocials,textWidthContactInformation;

         int textHeightHome ,textHeightProfile ,textHeightEDUCATION ,textHeightProjects ,textHeightCertificates ,textHeightTargets,textHeightWhy_me,textHeightSocials,textHeightContactInformation;


  int textYHome ,textYProfile ,textYEDUCATION ,textYProjects ,textYCertificates ,textYTargets,textYWhy_me,textYSocials,textYContactInformation;


         int HomeiconHeight= 40;
        int ProfileiconHeight= 40;
        int EDUCATIONiconHeight= 40;
        int PROJECTSIconHeight= 40;
        int CERTIFICATESIconHeight= 40;
        int TargetsiconHeight= 40;
        int Why_meIconHeight= 40;
        int SOCIALSIconHeight= 40;
        int CONTACTINFORMATIONIconHeight= 40;

         int HomeiconX, ProfileiconX, EDUCATIONiconX, PROJECTSIconX, CERTIFICATESIconX, TargetsiconX,Why_meIconX,CONTACTINFORMATIONIconX, SOCIALSIconX;

         float HomeiconY, ProfileiconY, EDUCATIONiconY, PROJECTSIconY, CERTIFICATESIconY, TargetsiconY, Why_meIconY,CONTACTINFORMATIONIconY, SOCIALSIconY;


// Define the proportional gap based on the window height
float proportionalGap = Height / 10.0;

SDL_Rect Bellicon;
SDL_Rect Homeicon;
SDL_Rect Profileicon;
SDL_Rect EDUCATIONicon;
SDL_Rect PROJECTSIcon;
SDL_Rect CERTIFICATESIcon;
SDL_Rect Targetsicon;
SDL_Rect Why_meIcon;
SDL_Rect SOCIALSIcon;
SDL_Rect CONTACTINFORMATIONIcon;



SDL_Rect outerBox ;
int boxWidth;
int boxHeight ;
int boxY ;
int boxX ;








public:
    NavigationMenu(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* NimbusRomFont)
        : State(window, renderer, NimbusRomFont) {
        SDL_StartTextInput();
        // Initialize other necessary variables if needed
    }

    virtual void handleEvents(SDL_Event& event) override {
  if (event.type == SDL_MOUSEWHEEL) {
        // Scrolling up
        if (event.wheel.y > 0) {
            scrollOffsetY -= 100; // Scroll speed
            if (scrollOffsetY < 0) scrollOffsetY = 0;
        }
        // Scrolling down
        else if (event.wheel.y < 0) {
            scrollOffsetY += 100; // Scroll speed
            if (scrollOffsetY > maxScrollOffset) scrollOffsetY = maxScrollOffset;
        }
    }

        if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_FINGERDOWN) {
        int mouseX, mouseY;

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            mouseX = event.button.x;
            mouseY = event.button.y;
        } else { // SDL_FINGERDOWN
            mouseX = event.tfinger.x * Width;
            mouseY = event.tfinger.y * Height;
        }

        std::cout << "Click at (" << mouseX << ", " << mouseY << ")" << std::endl;
  if (mouseX >= HomeiconX && mouseX <= HomeiconX + HomeiconWidth && mouseY >=HomeiconY && mouseY <= HomeiconY + HomeiconHeight) {
            std::cout << "Home Icon  clicked" << std::endl;
            changeState(MAIN_DASHBOARD);

}
else if (mouseX >= ProfileiconX && mouseX <= ProfileiconX + ProfileiconWidth && mouseY >= ProfileiconY && mouseY <= ProfileiconY +  ProfileiconHeight) {
            std::cout << "PROFILE_ Icon  clicked" << std::endl;
            changeState(PROFILE_SCREEN);

}

else if (mouseX >=EDUCATIONiconX && mouseX <= EDUCATIONiconX + EDUCATIONiconWidth && mouseY >= EDUCATIONiconY && mouseY <= EDUCATIONiconY + EDUCATIONiconHeight) {
            std::cout << " EDUCATION  Icon  clicked" << std::endl;
            changeState( EDUCATION_SCREEN);

}

else if (mouseX >= PROJECTSIconX && mouseX <= PROJECTSIconX + PROJECTSIconWidth && mouseY >= PROJECTSIconY && mouseY <= PROJECTSIconY + PROJECTSIconHeight) {
    std::cout << "PROJECTS Icon clicked" << std::endl;
    changeState(PROJECTS_SCREEN);
}
else if (mouseX >= CERTIFICATESIconX && mouseX <= CERTIFICATESIconX + CERTIFICATESIconWidth && mouseY >= CERTIFICATESIconY && mouseY <= CERTIFICATESIconY + CERTIFICATESIconHeight) {
    std::cout << "CERTIFICATES Icon clicked" << std::endl;
    changeState(CERTIFICATES_SCREEN);
}
else if (mouseX >= TargetsiconX && mouseX <= TargetsiconX + TargetsiconWidth && mouseY >= TargetsiconY && mouseY <= TargetsiconY + TargetsiconHeight) {
    std::cout << "TARGETS Icon clicked" << std::endl;
    changeState(TARGETS_SCREEN);
}
else if (mouseX >= Why_meIconX && mouseX <= Why_meIconX + Why_meIconWidth && mouseY >= Why_meIconY && mouseY <= Why_meIconY + Why_meIconHeight) {
    std::cout << "ELIGIBILITY_REASON Icon clicked" << std::endl;
    changeState(ELIGIBILITY_REASON_SCREEN);
}else if (mouseX >= SOCIALSIconX && mouseX <= SOCIALSIconX + SOCIALSIconWidth && mouseY >= SOCIALSIconY && mouseY <= SOCIALSIconY + SOCIALSIconHeight) {
    std::cout << "SOCIALS_SCREEN clicked" << std::endl;
    changeState(SOCIALS_SCREEN);
}else if (mouseX >= CONTACTINFORMATIONIconX && mouseX <= CONTACTINFORMATIONIconX + CONTACTINFORMATIONIconWidth && mouseY >= CONTACTINFORMATIONIconY && mouseY <= CONTACTINFORMATIONIconY + CONTACTINFORMATIONIconHeight) {
    std::cout << "CONTACT_INFORMATION_SCREEN Icon clicked" << std::endl;
    changeState(CONTACT_INFORMATION_SCREEN);
}


}
    }

   virtual  void update() override {
        // Update logic for System Health Screen if needed
    }

  virtual   void render() override {
    boxHeight = Height*7;
       maxScrollOffset = boxHeight ; 
// Set the position for the box
    boxY = 0 - scrollOffsetY;
 boxX = 53;
 boxWidth = Width-53;
  
 SDL_Rect outerBox = {boxX, boxY, boxWidth, boxHeight};
 // Set the color for the fill (black) and render the inside of the box

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    

    SDL_RenderFillRect(renderer, &outerBox);

    HomeiconY = 50;
    ProfileiconY = HomeiconY + proportionalGap;
    EDUCATIONiconY = ProfileiconY + proportionalGap;
    PROJECTSIconY = EDUCATIONiconY + proportionalGap;
    CERTIFICATESIconY = PROJECTSIconY + proportionalGap;
    TargetsiconY = CERTIFICATESIconY + proportionalGap;
    Why_meIconY = TargetsiconY + proportionalGap;
    SOCIALSIconY = Why_meIconY + proportionalGap;
    CONTACTINFORMATIONIconY = SOCIALSIconY + proportionalGap;

    const char *textHome = "Home";
    const char *textProfile = "Profile";
    const char *textEDUCATION = "Education";
    const char *textProjects = "Projects";
    const char *textCertificates = "Proofs";
    const char *textTargets = "Targets";
    const char *textWhy_me = "Why me?";
    const char *textSocials = "Socials";
    const char *textContactInformation = "Contact";

    TTF_SizeText(NimbusRomFont, textHome, &textWidthHome, &textHeightHome);
    TTF_SizeText(NimbusRomFont, textProfile, &textWidthProfile, &textHeightProfile);
    TTF_SizeText(NimbusRomFont, textEDUCATION, &textWidthEDUCATION, &textHeightEDUCATION);
    TTF_SizeText(NimbusRomFont, textProjects, &textWidthProjects, &textHeightProjects);
    TTF_SizeText(NimbusRomFont, textCertificates, &textWidthCertificates, &textHeightCertificates);
    TTF_SizeText(NimbusRomFont, textTargets, &textWidthTargets, &textHeightTargets);
    TTF_SizeText(NimbusRomFont, textWhy_me, &textWidthWhy_me, &textHeightWhy_me);
    TTF_SizeText(NimbusRomFont, textSocials, &textWidthSocials, &textHeightSocials);
    TTF_SizeText(NimbusRomFont, textContactInformation, &textWidthContactInformation, &textHeightContactInformation);

    HomeiconX = 0;
    ProfileiconX = 0;
    EDUCATIONiconX = 0;
    PROJECTSIconX = 0;
    CERTIFICATESIconX = 0;
    TargetsiconX = 0;
    Why_meIconX = 0;
    SOCIALSIconX = 0;
    CONTACTINFORMATIONIconX = 0;

    SDL_Texture *homeIconTexture = loadTexture("C:/NEW/assets/HOME.jpg", renderer);
    SDL_Texture *profileIconTexture = loadTexture("C:/NEW/assets/USERPROFILE.jpg", renderer);
    SDL_Texture *EDUCATIONIconTexture = loadTexture("C:/NEW/assets/EDUCATION.png", renderer);
    SDL_Texture *PROJECTSIconTexture = loadTexture("C:/NEW/assets/PROJECTS.png", renderer);
    SDL_Texture *CERTIFICATESIconTexture = loadTexture("C:/NEW/assets/CERTIFICATES.jpg", renderer);
    SDL_Texture *targetsIconTexture = loadTexture("C:/NEW/assets/targets.jpeg", renderer);
    SDL_Texture *Why_meIconTexture = loadTexture("C:/NEW/assets/Why_me.jpeg", renderer); ///
    SDL_Texture *SOCIALSIconTexture = loadTexture("C:/NEW/assets/SOCIALS.png", renderer);
    SDL_Texture *CONTACTINFORMATIONIconTexture = loadTexture("C:/NEW/assets/CONTACTINFORMATION.png", renderer);

    // Now create and render the icons with these calculated Y positions
    SDL_Rect Homeicon = {HomeiconX, (int)HomeiconY, HomeiconWidth, HomeiconHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderCopy(renderer, homeIconTexture, NULL, &Homeicon);

    SDL_Rect Profileicon = {ProfileiconX, (int)ProfileiconY, ProfileiconWidth, ProfileiconHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderCopy(renderer, profileIconTexture, NULL, &Profileicon);

    SDL_Rect EDUCATIONicon = {EDUCATIONiconX, (int)EDUCATIONiconY, EDUCATIONiconWidth, EDUCATIONiconHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderCopy(renderer, EDUCATIONIconTexture, NULL, &EDUCATIONicon);

    SDL_Rect PROJECTSIcon = {PROJECTSIconX, (int)PROJECTSIconY, PROJECTSIconWidth, PROJECTSIconHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderCopy(renderer, PROJECTSIconTexture, NULL, &PROJECTSIcon);

    SDL_Rect CERTIFICATESIcon = {CERTIFICATESIconX, (int)CERTIFICATESIconY, CERTIFICATESIconWidth, CERTIFICATESIconHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderCopy(renderer, CERTIFICATESIconTexture, NULL, &CERTIFICATESIcon);

    SDL_Rect Targetsicon = {TargetsiconX, (int)TargetsiconY, TargetsiconWidth, TargetsiconHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderCopy(renderer, targetsIconTexture, NULL, &Targetsicon);

    SDL_Rect Why_meIcon = {Why_meIconX, (int)Why_meIconY, Why_meIconWidth, Why_meIconHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderCopy(renderer, Why_meIconTexture, NULL, &Why_meIcon);

    SDL_Rect CONTACTINFORMATIONIcon = {CONTACTINFORMATIONIconX, (int)CONTACTINFORMATIONIconY, CONTACTINFORMATIONIconWidth, CONTACTINFORMATIONIconHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderCopy(renderer, CONTACTINFORMATIONIconTexture, NULL, &CONTACTINFORMATIONIcon);

    SDL_Rect SOCIALSIcon = {SOCIALSIconX, (int)SOCIALSIconY, SOCIALSIconWidth, SOCIALSIconHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderCopy(renderer, SOCIALSIconTexture, NULL, &SOCIALSIcon);


    // Render the icon labels
    renderText(textHome, HomeiconX, HomeiconY + HomeiconHeight, white,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 14, renderer);
    renderText(textProfile, ProfileiconX, ProfileiconY + ProfileiconHeight, white,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 14, renderer);
    renderText(textEDUCATION, EDUCATIONiconX, EDUCATIONiconY + EDUCATIONiconHeight, white,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 14, renderer);
    renderText(textProjects, PROJECTSIconX, PROJECTSIconY + PROJECTSIconHeight, white,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 14, renderer);
    renderText(textCertificates, CERTIFICATESIconX, CERTIFICATESIconY + CERTIFICATESIconHeight, white,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 14, renderer);
    renderText(textTargets, TargetsiconX, TargetsiconY + TargetsiconHeight, white,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 14, renderer);

    renderText(textWhy_me, Why_meIconX, Why_meIconY + Why_meIconHeight, white,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 14, renderer);
    renderText(textSocials, SOCIALSIconX, SOCIALSIconY + SOCIALSIconHeight, white,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 14, renderer);
    renderText(textContactInformation, CONTACTINFORMATIONIconX, CONTACTINFORMATIONIconY + CONTACTINFORMATIONIconHeight, white,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 14, renderer);

    // Cleanup smallFont
    

    SDL_DestroyTexture(homeIconTexture);
    SDL_DestroyTexture(profileIconTexture);
    SDL_DestroyTexture(EDUCATIONIconTexture);
    SDL_DestroyTexture(PROJECTSIconTexture);
    SDL_DestroyTexture(CERTIFICATESIconTexture);
    SDL_DestroyTexture(targetsIconTexture);
    SDL_DestroyTexture(Why_meIconTexture);
    SDL_DestroyTexture(SOCIALSIconTexture);
    SDL_DestroyTexture(CONTACTINFORMATIONIconTexture);

    // Quit SDL_image (somewhere in your cleanup code)
    IMG_Quit();
    }

   virtual  void cleanup() override {
        // Implement cleanup logic if needed
    }
};





// MainDashboardState class
class MainDashboardScreenState : public NavigationMenu {
    private:
      

    public:
        MainDashboardScreenState(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *NimbusRomFont) : NavigationMenu(window, renderer, NimbusRomFont)
 
        {


           SDL_StartTextInput();
    }
    
    void handleEvents(SDL_Event& event) override {
        NavigationMenu::handleEvents(event);
       
        }
     
    void update() override {
         NavigationMenu:: update();
    }
    
    void render() override {
        NavigationMenu::render();
       

        SDL_Color white = { 255, 255, 255, 255 };
// Get the dimensions of the text
    int textWidth, textHeight;
    TTF_SizeText(BrilliantDreamsFont, "RESUME", &textWidth, &textHeight);


const int textX = (Width-textWidth) / 2; // Center horizontally
    const int textY = (((Height - textHeight) / 2) - 250);

        renderText("RESUME",  textX, textY, white,"C:/NEW/assets/Brilliant Dreams.ttf",35, renderer);

}
    void cleanup() override {
        NavigationMenu::cleanup();
    }
};
// IntroScreenState class
class IntroScreenState : public State {
    private:
    std::string CompanyName,username,JobTitle;


bool enteringCompanyname;
bool enteringUsername;
bool enteringJobTitle;




   int CompanynameboxWidth=250;
     int usernameboxWidth=250;
   int  JobTitleboxWidth=250;
    
     int LaunchbuttonWidth=100;
     

 int CompanynameboxHeight=25 ;
     int usernameboxHeight=25 ; 
   int  JobTitleboxHeight=25 ;
   
     int LaunchbuttonHeight=30 ;
    

 int CompanynameboxX ;
     int usernameboxX ;
   int  JobTitleboxX ;
    
     int LaunchbuttonX ;
   
int boxX ;

 int CompanynameboxY;
     int usernameboxY; 
   int  JobTitleboxY;
    
     int LaunchbuttonY;
   
    int boxY ;

int boxPadding = 20;
int boxWidth = Width- 6* boxPadding;
    int boxHeight = Height - 10* boxPadding;
 SDL_Rect  CompanynameboxRect;
      SDL_Rect usernameboxRect; 
    SDL_Rect  JobTitleboxRect;
   
SDL_Rect outerBox ;



// Declare variables for text dimensions
    int textWidth1, textHeight1;
    int textWidth2, textHeight2;
    int textWidth3, textHeight3;
    int textWidth4, textHeight4;
    int textWidth5, textHeight5;
    int textWidth6, textHeight6;
    int textWidth7, textHeight7;
    int textWidth8a, textHeight8a;
    int textWidth8b, textHeight8b;

    int textWidth9, textHeight9;
    
    // Declare variables for dynamic text positions
    int textY1, textY2, textY3, textY4, textY5, textY6, textY7,textY8a,textY8b, textY9;

    int textX1, textX2, textX3, textX4, textX5, textX6, textX7, textX8a, textX8b, textX9;

 




public:
    IntroScreenState(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* NimbusRomFont,TTF_Font* BrockScript):State(window ,renderer, NimbusRomFont ),CompanyName(""), username(""), JobTitle(""), 
      enteringCompanyname(true), enteringUsername(false), enteringJobTitle(false)
         {
            
SDL_StartTextInput();

        // more Initialization if needed
    }
  


void handleEvents(SDL_Event& event) override {
    if (event.type == SDL_TEXTINPUT) {
        if (enteringCompanyname) {
            CompanyName += event.text.text;
        } else if (enteringUsername) {
            username += event.text.text;
        } else if (enteringJobTitle) {
            JobTitle += event.text.text;
        } 
    } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_BACKSPACE:
                if (enteringCompanyname && !CompanyName.empty()) {
                    CompanyName.pop_back();
                } else if (enteringUsername && !username.empty()) {
                    username.pop_back();
                } else if (enteringJobTitle && !JobTitle.empty()) {
                    JobTitle.pop_back();
                } 
                break;

case SDLK_RETURN:
    if (enteringCompanyname && !CompanyName.empty()) {
        enteringCompanyname = false;
        enteringUsername = true;
    } else if (enteringUsername && !username.empty()) {
        enteringUsername = false;
        enteringJobTitle = true;
    } else if (enteringJobTitle && !JobTitle.empty()) {
        enteringJobTitle = false;
    
        saveUserDataToDatabase(); 


        changeState(MAIN_DASHBOARD);
    }
    break;
            default:
                break;
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_FINGERDOWN) {
        int mouseX, mouseY;

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            mouseX = event.button.x;
            mouseY = event.button.y;
        } else { // SDL_FINGERDOWN
            mouseX = event.tfinger.x * Width;
            mouseY = event.tfinger.y * Height;
        }

        std::cout << "Click at (" << mouseX << ", " << mouseY << ")" << std::endl;

        if (mouseX >= CompanynameboxX && mouseX <= CompanynameboxX + CompanynameboxWidth && mouseY >= CompanynameboxY && mouseY <= CompanynameboxY + CompanynameboxHeight) {
            enteringCompanyname = true;
            enteringUsername = false;
            enteringJobTitle = false;
          
        } else if (mouseX >= usernameboxX && mouseX <= usernameboxX + usernameboxWidth && mouseY >= usernameboxY && mouseY <= usernameboxY + usernameboxHeight) {
            enteringCompanyname = false;
            enteringUsername = true;
            enteringJobTitle = false;
          
        } else if (mouseX >= JobTitleboxX && mouseX <= JobTitleboxX + JobTitleboxWidth && mouseY >= JobTitleboxY && mouseY <= JobTitleboxY + JobTitleboxHeight) {
            enteringCompanyname = false;
            enteringUsername = false;
            enteringJobTitle = true;
          
        } 
         else if (mouseX >= textX7 && mouseX <= textX7 + textWidth7 && mouseY >= textY7 && mouseY <= textY7 + textHeight7) {
            std::cout << "launch  button clicked" << std::endl;
              saveUserDataToDatabase(); 
            changeState(MAIN_DASHBOARD);
        }
    }
}


    
    void update() override {
        // Update logic for Launch Screen
    }
    
    void render() override {

SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color grey = { 100, 100, 100, 255 };
    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Color darkgreen = { 0, 50, 0, 255  }; 
    SDL_Color maroon = { 128, 0, 0, 255 };
 
// Calculate dynamic positions based on window size
  textY1 = boxY - 15; 
    textY2 = boxY + (Height / 30) ;


    textY3 = boxY + (Height / 30) + 60;
    textY4 = boxY + (Height / 30) + 90;
    textY5 = boxY + (Height / 30) + 120;
    textY6 = boxY + (Height / 30) + 150;


    textY7 = boxY + (Height / 30) + 190;
    textY8a = boxY + (Height /30) + 240;
    textY8b = boxY + (Height /30) + 280;
    textY9 = boxY + (Height / 30) + 300;

 CompanynameboxY = textY3-10    ;
      usernameboxY=  textY4-10 ;
     JobTitleboxY= textY5 -10 ;
   LaunchbuttonY=textY7-11  ;
     
      



boxY = (Height - boxHeight) / 2;

   
       const char* text1 = "Your Basic Details ";

std::string text3 = "Company Name" + CompanyName;
std::string text4 = "Your  Name" + username;
std::string text5 = "Job Title  " + JobTitle;

const char* text7 = "   Launch"; // Launchbutton
const char* text8a = "Unveil to see what I can bring to the table ";
    const char* text8b = "&&    ";
const char* text9 = "Why It's Beneficial for Both of Us ?"; 


// Get the dimensions of the text
TTF_SizeText(NimbusRomFont, text1, &textWidth1, &textHeight1);

TTF_SizeText(NimbusRomFont, text3.c_str(), &textWidth3, &textHeight3);
TTF_SizeText(NimbusRomFont, text4.c_str(), &textWidth4, &textHeight4);


TTF_SizeText(NimbusRomFont, text5.c_str(), &textWidth5, &textHeight5);

TTF_SizeText(NimbusRomFont, text7, &textWidth7, &textHeight7);
 TTF_SizeText(BrockScript, text8a, &textWidth8a, &textHeight8a);
 TTF_SizeText(NimbusRomFont, text8b, &textWidth8b, &textHeight8b);
 TTF_SizeText(BrockScript, text9, &textWidth9, &textHeight9);



// Calculate x positions to center the text within the box
     textX1 = (Width - textWidth1) / 2;
    textX2 = boxX + (boxWidth - textWidth2) / 2;
    textX3 = boxX + (boxWidth - textWidth3) / 2;
    textX4 = boxX + (boxWidth - textWidth4) / 2;
    textX5 = boxX + (boxWidth - textWidth5) / 2;
    textX6 = boxX + (boxWidth - textWidth6) / 2;
    textX7 = boxX + (boxWidth - textWidth7) / 2;
    textX8a = boxX + (boxWidth - textWidth8a) / 2;
    textX8b = boxX + (boxWidth - textWidth8b) / 2;
    textX9 = boxX + (boxWidth - textWidth9) / 2;


CompanynameboxX =  boxX + (boxWidth - CompanynameboxWidth) / 2  ;
      usernameboxX=  boxX + (boxWidth -  usernameboxWidth) / 2  ;
     JobTitleboxX=   boxX + (boxWidth - JobTitleboxWidth) / 2  ;
    


      LaunchbuttonX=  boxX + (boxWidth - LaunchbuttonWidth) / 2  ;
boxX = (Width - boxWidth) / 2;



SDL_Rect outerBox = { boxX, boxY, boxWidth, boxHeight };
SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &outerBox);



   // Define positions and dimensions of input boxes
    SDL_Rect Companynamebox = { CompanynameboxX, CompanynameboxY, CompanynameboxWidth, CompanynameboxHeight };


// Set the color for the fill (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Render the username input box filled with white color
    SDL_RenderFillRect(renderer, &Companynamebox);
//text3
    SDL_Rect usernamebox = { usernameboxX, usernameboxY, usernameboxWidth, usernameboxHeight };


// Set the color for the fill (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Render the username input box filled with white color
    SDL_RenderFillRect(renderer, &usernamebox);

    //text4
    SDL_Rect JobTitlebox = { JobTitleboxX, JobTitleboxY, JobTitleboxWidth, JobTitleboxHeight };
// Set the color for the fill (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // Render the username input box filled with white color
    SDL_RenderFillRect(renderer, &JobTitlebox);
//text5


          renderRoundedRect(renderer, LaunchbuttonX, LaunchbuttonY, LaunchbuttonWidth, LaunchbuttonHeight, radius,darkgreen);


        renderText(text1, textX1, textY1, white,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 26,renderer);
      
        renderText(text7, textX7, textY7, white,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 20,renderer);
        renderText(text8a, textX8a, textY8a, grey,"C:/NEW/assets/BrockScript.ttf",25, renderer);
        renderText(text8b, textX8b, textY8b, grey,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 25,renderer);
        renderText(text9, textX9, textY9, grey, "C:/NEW/assets/BrockScript.ttf",25,renderer);
       

  
// Render Companyname 
if (enteringCompanyname && !CompanyName.empty()) {
    renderText(CompanyName.c_str(), CompanynameboxX, CompanynameboxY, black,"C:/NEW/assets/NimbusRomNo9L-Reg.otf",20, renderer);
} else {
    // Render placeholder text only if CompanyName is empty
    if (CompanyName.empty()) {
        renderText("Company Name ", textX3, textY3, grey,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 20,renderer);
    } else {
        renderText(CompanyName.c_str(), CompanynameboxX, CompanynameboxY, black,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 20,renderer);
    }
}


  


    // Render user ID
    if (enteringUsername && !username.empty()) {
        renderText(username.c_str(), usernameboxX, usernameboxY, black,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 20,renderer);
    } else {

 if (username.empty()) {
         renderText("User ID ", textX4, textY4, grey,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 20,renderer);
    } else {
        renderText(username.c_str(), usernameboxX, usernameboxY, black,"C:/NEW/assets/NimbusRomNo9L-Reg.otf",20, renderer);
    }
}
    

    // Render JobTitle
    if (enteringJobTitle && !JobTitle.empty()) {
        renderText(JobTitle.c_str(), JobTitleboxX, JobTitleboxY, black,"C:/NEW/assets/NimbusRomNo9L-Reg.otf",20, renderer);
    } else {
         if (JobTitle.empty()) {
            
        renderText("JobTitle ", textX5, textY5, grey,"C:/NEW/assets/NimbusRomNo9L-Reg.otf",20, renderer);
    } else {
       renderText(JobTitle.c_str(), JobTitleboxX, JobTitleboxY, black,"C:/NEW/assets/NimbusRomNo9L-Reg.otf", 20,renderer);
    }
}
   


   

    }

    
    void cleanup() override {
        // Implement cleanup logic 
    }

// Method to save user data to database
  void saveUserDataToDatabase() {
    // Validate input
    if (CompanyName.empty() || username.empty() || JobTitle.empty() ) {
        std::cerr << "Error: All fields are required." << std::endl;
        return;
    }
    


    // Create User object
    User newUser(CompanyName, username, JobTitle);

    // Save to database
    if (insertUser(newUser)) {
        std::cout << "User registered successfully." << std::endl;
    } else {
        std::cerr << "Error: Failed to register user." << std::endl;
    }
}

};
class ProfileScreenState : public NavigationMenu {
private:
    SDL_Color white = { 255, 255, 255, 255 };

public:
    ProfileScreenState(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* NimbusRomFont)
        : NavigationMenu(window, renderer, NimbusRomFont) {
        SDL_StartTextInput();
    }

    void handleEvents(SDL_Event& event) override {
        NavigationMenu::handleEvents(event);
    }

    void update() override {
        NavigationMenu::update();
    }

   void render() override {
    NavigationMenu::render();

    // Fixed Y positions for each section
    int sectionY = 50; // Y position for the first section
    int lineSpacing = 25; // Space between lines of text

    // Helper function to render centered text
    auto renderCenteredText = [&](const std::string& text, int y, SDL_Color color, const std::string& fontPath, int fontSize) {
        // Load the font
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            return;
        }

        // Calculate the width of the text
        int textWidth;
        int textHeight;
        TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

        // Calculate the X position to center the text
        int textX = (Width - textWidth) / 2;

        // Render the text using the renderText function
        renderText(text, textX, y, color, fontPath, fontSize, renderer);

        // Close the font
        TTF_CloseFont(font);
    };

    // Render Personal Information Section
    renderCenteredText("Overview", sectionY, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 36);

    renderCenteredText("I'm Ayesha Siddiqa, Who is currently a 1st semester ", sectionY + 2 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
    
    renderCenteredText("AI Student at Air University Islamabad", sectionY + 3 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
std::string paragraph = "So, you're figuring out if I'm the right match for this internship, huh? Well, let me make things interesting a bit! I've built this cross-platform app from scratch using C++ and SDL. It's like my personal showcase, with a tech twist and your basic details are stored in an SQLite database. If you're curious about the magic behind this app, you'll find the code in the 'Code' section.\n\n"
"Got any feedback, spotted any bugs, or just want to inform me when I can join to collaborate? Don't hesitate to hit me up! You can find my social media links and contact info in the 'Socials' & 'Contact' section of the app. I'm all ears for your insights.\n\n"
"Please do point out if you find any mistakes or have any suggestions for improvements. Even if I don't get the internship, your feedback is still valued because that's how we learn.";

// Render the paragraph centered on the screen
renderParagraph(paragraph, sectionY + 4 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16, renderer, Width);


    renderCenteredText("Take a moment to explore the other sections too. And  thanks a bunch for your time!", sectionY + 13 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);

    // Present rendered content
    SDL_RenderPresent(renderer);
}

    void cleanup() override {
        NavigationMenu::cleanup();
    }
};


///////////////////////////////////////////////////////////////////
class EducationScreenState : public NavigationMenu {
private:
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color grey = { 100, 100, 100, 255 };
  int pgciconWidth= 55;
        int airiconWidth= 55;
        int youtubeiconWidth= 55;
        int booksiconWidth= 55;

  int pgciconHeight= 55;
        int airiconHeight= 55;
        int youtubeiconHeight= 55;
        int booksiconHeight= 55;

 int pgciconX, airiconX, youtubeiconX, booksiconX;

 float pgciconY, airiconY, youtubeiconY, booksiconY;


SDL_Rect pgcicon;
SDL_Rect airicon;
SDL_Rect youtubeicon;
SDL_Rect booksicon;







  
public:
    EducationScreenState(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* NimbusRomFont)
        : NavigationMenu(window, renderer, NimbusRomFont) {
        SDL_StartTextInput();
    }

   
    void handleEvents(SDL_Event& event) override {
         NavigationMenu::handleEvents(event);
    }

    void update() override {
        NavigationMenu:: update();
    }

  
   void render() override {
    NavigationMenu::render();

    // Fixed Y positions for each section
    int sectionY = 50; // Y position for the first section
    int lineSpacing = 25; // Space between lines of text

    // Helper function to render centered text
    auto renderCenteredText = [&](const std::string& text, int y, SDL_Color color, const std::string& fontPath, int fontSize) {
        // Load the font
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            return;
        }

        // Calculate the width of the text
        int textWidth;
        int textHeight;
        TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

        // Calculate the X position to center the text
        int textX = (Width - textWidth) / 2;

        // Render the text using the renderText function
        renderText(text, textX, y, color, fontPath, fontSize, renderer);

        // Close the font
        TTF_CloseFont(font);
    };

    // Render Personal Information Section
    renderCenteredText(" Formal Credentials", sectionY, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 36);

   pgciconY = sectionY;
airiconY =sectionY;
pgciconX=200;
 airiconX =550;
std::string paragraph1 = 
"SSC: Completed\n"
"Marks:  90% , A+ Grade \n"
"College : Punjab  Group Of Colleges\n"
"HSSC: Completed\n"
"Marks:  80% , A Grade\n"
"College : Punjab  Group Of Colleges\n"
"BS: Ongoing .....\n"
"Current Semester: Artificial Intelligence 1st Semester \n"
"University: Air University Islamabad\n";

// Render the paragraph centered on the screen
renderParagraph(paragraph1, (sectionY +2 * lineSpacing), white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16, renderer, Width);


   renderCenteredText("Informal Credentials", (sectionY +11 * lineSpacing), white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 36);

youtubeiconY=  (sectionY +11 * lineSpacing);   
booksiconY=   (sectionY +11 * lineSpacing);

youtubeiconX =200;
booksiconX=560;
renderCenteredText("Youtube , Google  ,  Documentations/Books ", (sectionY + 13 * lineSpacing) , white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 20);


   
std::string paragraph2 = "What  i have learnt is:\n"
"Programming Languages : C and C++(Not an expert yet ,but but getting better daily) \n"
"Development Libraries :SDL + other  libraries as needed (which means i can code from skratch)   \n"
"Database : SQLite (for smaller projects) But focusing  more on PostgreSQL(for big projects) \n"
"AI/ML with C++:tensor flow,cuda,opencv(Not an expert yet, but getting better daily)\n"
"Tools: Git, GitHub, &&  VS Code, Visual Studio, && .........  && all those other developer essentials that are like a coffee machine for a programmer, that are obviously important and always in use.    \n"
"Medical Sciences: Human anatomy(especailly Neuroanatomy),Human physiology \n"
"And that's it . \n";


// Render the paragraph centered on the screen
renderParagraph(paragraph2, (sectionY +14 * lineSpacing), white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16, renderer, Width);

renderCenteredText("Now, I think you should check out projects sections to see what I've done with it.", sectionY +30 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);


SDL_Texture *pgciconTexture = loadTexture("C:/NEW/assets/pgc.jpeg", renderer);
SDL_Texture *airiconTexture = loadTexture("C:/NEW/assets/air.jpeg", renderer);
SDL_Texture *youtubeiconTexture = loadTexture("C:/NEW/assets/youtube.png", renderer);
SDL_Texture *booksiconTexture = loadTexture("C:/NEW/assets/books.png", renderer);



SDL_Rect pgcicon={pgciconX,(int)pgciconY,pgciconWidth,pgciconHeight};
SDL_SetRenderDrawColor(renderer,0,0,0,255);
SDL_RenderCopy(renderer,pgciconTexture,NULL,&pgcicon);

SDL_Rect airicon={airiconX,(int)airiconY,airiconWidth,airiconHeight};
SDL_SetRenderDrawColor(renderer,0,0,0,255);
SDL_RenderCopy(renderer,airiconTexture,NULL,&airicon);

SDL_Rect youtubeicon={youtubeiconX,(int)youtubeiconY,youtubeiconWidth,youtubeiconHeight};
SDL_SetRenderDrawColor(renderer,0,0,0,255);
SDL_RenderCopy(renderer,youtubeiconTexture,NULL,&youtubeicon);

SDL_Rect booksicon={booksiconX,(int)booksiconY,booksiconWidth,booksiconHeight};
SDL_SetRenderDrawColor(renderer,0,0,0,255);
SDL_RenderCopy(renderer,booksiconTexture,NULL,&booksicon);




    // Present rendered content
    SDL_RenderPresent(renderer);
}


    void cleanup() override {
         NavigationMenu::cleanup();
    }
};





class ProjectsScreenState : public NavigationMenu {
private:
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color grey = { 100, 100, 100, 255 };
    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Color darkgreen = { 0, 50, 0, 255 };
    SDL_Color maroon = { 128, 0, 0, 255 };

public:
    ProjectsScreenState(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* NimbusRomFont)
        : NavigationMenu(window, renderer, NimbusRomFont) {
        SDL_StartTextInput();
        // Initialize other necessary variables if needed
    }

    void handleEvents(SDL_Event& event) override {
         NavigationMenu::handleEvents(event);
    }

    void update() override {
        NavigationMenu:: update();
    }

    
   void render() override {
    NavigationMenu::render();

    // Fixed Y positions for each section
    int sectionY = 50; // Y position for the first section
    int lineSpacing = 25; // Space between lines of text

    // Helper function to render centered text
    auto renderCenteredText = [&](const std::string& text, int y, SDL_Color color, const std::string& fontPath, int fontSize) {
        // Load the font
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            return;
        }

        // Calculate the width of the text
        int textWidth;
        int textHeight;
        TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

        // Calculate the X position to center the text
        int textX = (Width - textWidth) / 2;

        // Render the text using the renderText function
        renderText(text, textX, y, color, fontPath, fontSize, renderer);

        // Close the font
        TTF_CloseFont(font);
    };

    // Render Personal Information Section
    renderCenteredText("Overview", sectionY, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 36);

    renderCenteredText("I'm Ayesha Siddiqa, Who is currently a 1st semester ", sectionY + 2 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
    
    renderCenteredText("AI Student at Air University Islamabad", sectionY + 3 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
std::string paragraph = "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp";

// Render the paragraph centered on the screen
renderParagraph(paragraph, sectionY + 4 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16, renderer, Width);


    renderCenteredText("Take a moment to explore the other sections too. And  thanks a bunch for your time!", sectionY + 13 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);

    // Present rendered content
    SDL_RenderPresent(renderer);
}

    void cleanup() override {
         NavigationMenu::cleanup();
    }
};

class CertificatesScreenState : public NavigationMenu {
private:
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color grey = { 100, 100, 100, 255 };
    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Color darkgreen = { 0, 50, 0, 255 };
    SDL_Color maroon = { 128, 0, 0, 255 };

public:
   CertificatesScreenState(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* NimbusRomFont)
        : NavigationMenu(window, renderer, NimbusRomFont) {
        SDL_StartTextInput();
        // Initialize other necessary variables if needed
    }

    void handleEvents(SDL_Event& event) override {
        NavigationMenu::handleEvents(event);
    }

    void update() override {
        NavigationMenu:: update();
    }

    
   void render() override {
    NavigationMenu::render();

    // Fixed Y positions for each section
    int sectionY = 50; // Y position for the first section
    int lineSpacing = 25; // Space between lines of text

    // Helper function to render centered text
    auto renderCenteredText = [&](const std::string& text, int y, SDL_Color color, const std::string& fontPath, int fontSize) {
        // Load the font
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            return;
        }

        // Calculate the width of the text
        int textWidth;
        int textHeight;
        TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

        // Calculate the X position to center the text
        int textX = (Width - textWidth) / 2;

        // Render the text using the renderText function
        renderText(text, textX, y, color, fontPath, fontSize, renderer);

        // Close the font
        TTF_CloseFont(font);
    };

    // Render Personal Information Section
    renderCenteredText("Overview", sectionY, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 36);

    renderCenteredText("I'm Ayesha Siddiqa, Who is currently a 1st semester ", sectionY + 2 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
    
    renderCenteredText("AI Student at Air University Islamabad", sectionY + 3 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
std::string paragraph = "ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";

// Render the paragraph centered on the screen
renderParagraph(paragraph, sectionY + 4 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16, renderer, Width);


    renderCenteredText("Take a moment to explore the other sections too. And  thanks a bunch for your time!", sectionY + 13 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);

    // Present rendered content
    SDL_RenderPresent(renderer);
}

    void cleanup() override {
         NavigationMenu::cleanup();}
};

class TargetsScreenState : public NavigationMenu{
private:
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color grey = { 100, 100, 100, 255 };
    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Color darkgreen = { 0, 50, 0, 255 };
    SDL_Color maroon = { 128, 0, 0, 255 };

public:
    TargetsScreenState(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* NimbusRomFont)
        : NavigationMenu(window, renderer, NimbusRomFont) {
        SDL_StartTextInput();
        // Initialize other necessary variables if needed
    }

    void handleEvents(SDL_Event& event) override {
         NavigationMenu::handleEvents(event);
    }

    void update() override {
        NavigationMenu:: update();
    }

 
   void render() override {
    NavigationMenu::render();

    // Fixed Y positions for each section
    int sectionY = 50; // Y position for the first section
    int lineSpacing = 25; // Space between lines of text

    // Helper function to render centered text
    auto renderCenteredText = [&](const std::string& text, int y, SDL_Color color, const std::string& fontPath, int fontSize) {
        // Load the font
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            return;
        }

        // Calculate the width of the text
        int textWidth;
        int textHeight;
        TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

        // Calculate the X position to center the text
        int textX = (Width - textWidth) / 2;

        // Render the text using the renderText function
        renderText(text, textX, y, color, fontPath, fontSize, renderer);

        // Close the font
        TTF_CloseFont(font);
    };

    // Render Personal Information Section
    renderCenteredText("ttttttttttttttttt", sectionY, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 36);

    renderCenteredText("ttttttttttttttttttttttttttttttttttttttt ", sectionY + 2 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
    
    renderCenteredText("ttttttttttttttttttttttttttttttttttttttttttttt", sectionY + 3 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
std::string paragraph = "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt";

// Render the paragraph centered on the screen
renderParagraph(paragraph, sectionY + 4 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16, renderer, Width);


    renderCenteredText("Take a moment to explore the other sections too. And  thanks a bunch for your time!", sectionY + 13 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);

    // Present rendered content
    SDL_RenderPresent(renderer);
}

    void cleanup() override {
        NavigationMenu::cleanup();
    }
};



class Why_meScreenState : public NavigationMenu {
private:
    const SDL_Color white = { 255, 255, 255, 255 };
    const SDL_Color black = { 0, 0, 0, 255 };
    const SDL_Color green = { 0, 255, 50, 255 };
    const SDL_Color red = { 255, 0, 0, 255 };
    const SDL_Color boxColor = { 50, 50, 50, 255 }; // 

public:
    Why_meScreenState (SDL_Window* window, SDL_Renderer* renderer, TTF_Font* NimbusRomFont)
        : NavigationMenu(window, renderer, NimbusRomFont) {
        SDL_StartTextInput();
        
    }

   
    void handleEvents(SDL_Event& event) override {
         NavigationMenu::handleEvents(event);
    }

    void update() override {
        NavigationMenu:: update();
    }

   
   void render() override {
    NavigationMenu::render();

    // Fixed Y positions for each section
    int sectionY = 50; // Y position for the first section
    int lineSpacing = 25; // Space between lines of text

    // Helper function to render centered text
    auto renderCenteredText = [&](const std::string& text, int y, SDL_Color color, const std::string& fontPath, int fontSize) {
        // Load the font
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            return;
        }

        // Calculate the width of the text
        int textWidth;
        int textHeight;
        TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

        // Calculate the X position to center the text
        int textX = (Width - textWidth) / 2;

        // Render the text using the renderText function
        renderText(text, textX, y, color, fontPath, fontSize, renderer);

        // Close the font
        TTF_CloseFont(font);
    };

    // Render Personal Information Section
    renderCenteredText("wwwwwwwww", sectionY, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 36);

    renderCenteredText("wwwwwwwwwwwwwwwwwwwwwwwww ", sectionY + 2 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
    
    renderCenteredText("wwwwwwwwwwwwwwwwwwww", sectionY + 3 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
std::string paragraph = "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww";

// Render the paragraph centered on the screen
renderParagraph(paragraph, sectionY + 4 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16, renderer, Width);


    renderCenteredText("Take a moment to explore the other sections too. And  thanks a bunch for your time!", sectionY + 13 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);

    // Present rendered content
    SDL_RenderPresent(renderer);
}


    void cleanup() override {
         NavigationMenu::cleanup();
    }
};




class SocialsScreenState : public NavigationMenu{
private:
     SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color grey = { 100, 100, 100, 255 };
    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Color darkgreen = { 0, 50, 0, 255  }; 
    SDL_Color maroon = { 128, 0, 0, 255 };
 
public:
   SocialsScreenState(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* NimbusRomFont)
        : NavigationMenu(window, renderer, NimbusRomFont) {
        SDL_StartTextInput();
        
    }

   
    void handleEvents(SDL_Event& event) override {
         NavigationMenu::handleEvents(event);
    }

    void update() override {
        NavigationMenu:: update();
    }

   
   void render() override {
    NavigationMenu::render();

    // Fixed Y positions for each section
    int sectionY = 50; // Y position for the first section
    int lineSpacing = 25; // Space between lines of text

    // Helper function to render centered text
    auto renderCenteredText = [&](const std::string& text, int y, SDL_Color color, const std::string& fontPath, int fontSize) {
        // Load the font
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            return;
        }

        // Calculate the width of the text
        int textWidth;
        int textHeight;
        TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

        // Calculate the X position to center the text
        int textX = (Width - textWidth) / 2;

        // Render the text using the renderText function
        renderText(text, textX, y, color, fontPath, fontSize, renderer);

        // Close the font
        TTF_CloseFont(font);
    };

    // Render Personal Information Section
    renderCenteredText("Overview", sectionY, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 36);

    renderCenteredText("I'm Ayesha Siddiqa, Who is currently a 1st semester ", sectionY + 2 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
    
    renderCenteredText("AI Student at Air University Islamabad", sectionY + 3 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
std::string paragraph = "sssssssssssssssssssssssssssssssssss";

// Render the paragraph centered on the screen
renderParagraph(paragraph, sectionY + 4 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16, renderer, Width);


    renderCenteredText("Take a moment to explore the other sections too. And  thanks a bunch for your time!", sectionY + 13 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);

    // Present rendered content
    SDL_RenderPresent(renderer);
}

    void cleanup() override {
         NavigationMenu::cleanup();
    }
   
};

class ContactInofrmationScreenState : public NavigationMenu {
private:
     SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color grey = { 100, 100, 100, 255 };
    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Color darkgreen = { 0, 50, 0, 255  }; 
    SDL_Color maroon = { 128, 0, 0, 255 };
 
public:
   ContactInofrmationScreenState(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* NimbusRomFont)
        : NavigationMenu(window, renderer, NimbusRomFont) {
        SDL_StartTextInput();
        
    }

   
    void handleEvents(SDL_Event& event) override {
         NavigationMenu::handleEvents(event);
    }

    void update() override {
        NavigationMenu:: update();
    }

   
   void render() override {
    NavigationMenu::render();

    // Fixed Y positions for each section
    int sectionY = 50; // Y position for the first section
    int lineSpacing = 25; // Space between lines of text

    // Helper function to render centered text
    auto renderCenteredText = [&](const std::string& text, int y, SDL_Color color, const std::string& fontPath, int fontSize) {
        // Load the font
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            return;
        }

        // Calculate the width of the text
        int textWidth;
        int textHeight;
        TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

        // Calculate the X position to center the text
        int textX = (Width - textWidth) / 2;

        // Render the text using the renderText function
        renderText(text, textX, y, color, fontPath, fontSize, renderer);

        // Close the font
        TTF_CloseFont(font);
    };

    // Render Personal Information Section
    renderCenteredText("nnnnnnnnnnnn", sectionY, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 36);

    renderCenteredText("nnnnnnnnnnnnnnnnnnn ", sectionY + 2 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
    
    renderCenteredText("nnnnnnnnnnnnnnnnnnnnn", sectionY + 3 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);
std::string paragraph = "cnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn.";

// Render the paragraph centered on the screen
renderParagraph(paragraph, sectionY + 4 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16, renderer, Width);


    renderCenteredText("Take a moment to explore the other sections too. And  thanks a bunch for your time!", sectionY + 13 * lineSpacing, white, "C:/NEW/assets/NimbusRomNo9L-Reg.otf", 16);

    // Present rendered content
    SDL_RenderPresent(renderer);
}

    void cleanup() override {
         NavigationMenu::cleanup();
    }
   
};



std::map<AppState, SDL_Color> backgroundColors = {
    {SPLASH_SCREEN, {0, 0, 0, 255}},        // Black
    {INTRO_SCREEN,{50, 50, 50, 255}},    // Dark Grey
   
    {MAIN_DASHBOARD, {50,50,50 ,255}},        // Black
  
    {PROFILE_SCREEN, {50, 50, 50, 255}},    // Dark Grey
    {EDUCATION_SCREEN, {50, 50, 50, 255}},    // Dark Grey
    {PROJECTS_SCREEN, {50, 50, 50, 255}},    // Dark Grey
    {CERTIFICATES_SCREEN, {50, 50, 50, 255}},    // Dark Grey
    {TARGETS_SCREEN, {50, 50, 50, 255}},    // Dark Grey
    {ELIGIBILITY_REASON_SCREEN, {50, 50, 50, 255}},    // Dark Greyz
    {SOCIALS_SCREEN  , {50, 50, 50, 255}},    // Dark Greyz
    {CONTACT_INFORMATION_SCREEN      , {50, 50, 50, 255}},    // Dark Greyz
   
    
    // Add other colors for other states as needed
};



// Function to change the current state
void changeState(AppState newState) {
      std::cout << "Changing state to: " << newState << std::endl;
    
  // Clean up the current state
    if (currentStateInstance) {
        currentStateInstance->cleanup();
        currentStateInstance.reset();
    }


    switch (newState) {
        case SPLASH_SCREEN:
            currentStateInstance = std::make_unique<SplashScreenState>(window, renderer, NimbusRomFont);
            break;
        case INTRO_SCREEN:
            currentStateInstance = std::make_unique<IntroScreenState>(window, renderer, NimbusRomFont,BrockScript);
            break;
      
        case MAIN_DASHBOARD:
            currentStateInstance = std::make_unique<MainDashboardScreenState>(window, renderer, NimbusRomFont);
            break; 

        case PROFILE_SCREEN:
           currentStateInstance = std::make_unique<ProfileScreenState>(window, renderer, NimbusRomFont);
            break;
             case EDUCATION_SCREEN:
            currentStateInstance = std::make_unique<EducationScreenState>(window, renderer, NimbusRomFont);
            break;
             case PROJECTS_SCREEN:
            currentStateInstance = std::make_unique<ProjectsScreenState>(window, renderer, NimbusRomFont);
            break;
             case CERTIFICATES_SCREEN:
            currentStateInstance = std::make_unique<CertificatesScreenState>(window, renderer, NimbusRomFont);
            break;
             case TARGETS_SCREEN:
            currentStateInstance = std::make_unique<TargetsScreenState>(window, renderer, NimbusRomFont);
            break;
             case ELIGIBILITY_REASON_SCREEN:
           currentStateInstance = std::make_unique<Why_meScreenState>(window, renderer, NimbusRomFont);
            break;
            case SOCIALS_SCREEN:
            currentStateInstance = std::make_unique<SocialsScreenState>(window, renderer, NimbusRomFont);
            break;
             case CONTACT_INFORMATION_SCREEN:
            currentStateInstance = std::make_unique<ContactInofrmationScreenState>(window, renderer, NimbusRomFont);
            break;
             
           
        default:
            // Handle default case or error condition
            break;
    }
    // Change window title based on newState
    SDL_SetWindowTitle(window, screenNames[newState]);

 // Set the background color
    SDL_Color bgColor = backgroundColors[newState];
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(renderer);


    
    SDL_RenderPresent(renderer);



    currentState = newState;
}

// Main function
int main(int argc, char* argv[]) {




// Initialize SDL_image (somewhere in your initialization code)
if (IMG_Init(IMG_INIT_PNG) == 0) {
    std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
    return -1;
}



// intialize libsodium
if (sodium_init() < 0) {
    std::cerr << "Failed to initialize libsodium" << std::endl;
    return -1; // or handle the error appropriately
}
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Initialize SDL TTF
    if (TTF_Init() == -1) {
        std::cerr << "SDL TTF initialization failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create SDL window
    window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width,Height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Create SDL renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Load Digital-7 font
    NunitoFont = TTF_OpenFont("C:/NEW/assets/Nunito-Regular.ttf", 18);
    if (!NunitoFont) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

     // Load bold nunito font
    NimbusRomFont = TTF_OpenFont("C:/NEW/assets/NimbusRomNo9L-Reg.otf", 25);
    BrockScript = TTF_OpenFont("C:/NEW/assets/BrockScript.ttf", 25);
    middlesizeNimbusRomFont  = TTF_OpenFont("C:/NEW/assets/NimbusRomNo9L-Reg.otf", 14);
   BrilliantDreamsFont  = TTF_OpenFont("C:/NEW/assets/Brilliant Dreams.ttf", 35);
    if (!NimbusRomFont) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
 // Gather user input (replace with actual input logic)
    std::string CompanyName = "user@example.com"; // Replace with actual input
    std::string username = "exampleUser";          // Replace with actual input
    std::string JobTitle = "JobTitle123"; // Replace with actual input

    // Initialize currentUser with gathered values
    currentUser = std::make_unique<User>(CompanyName, username, JobTitle);

    // Access properties using getters
    std::string currentCompanyname = currentUser->getCompanyName();
    std::string currentUsername = currentUser->getUsername();
    std::string currentJobTitle = currentUser->getJobTitle();

    // Output to check values (for debugging or display)
    std::cout << "Companyname: " << currentCompanyname << std::endl;
    std::cout << "Username: " << currentUsername << std::endl;
    std::cout << "JobTitle: " << currentJobTitle << std::endl;
// Change state to SPLASH_SCREEN
changeState(SPLASH_SCREEN);
    // Initialize start time
    startTime = SDL_GetTicks();
    
    // Event loop
    bool quit = false;
    SDL_Event e;
    

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_WINDOWEVENT) {
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        // Handle window resize event
                        Width = e.window.data1;
                        Height = e.window.data2;
                      
                       
                        break;
                    default:
                        break;
                }
            } else {
                currentStateInstance->handleEvents(e);
            }
        }

        // // Clear the screen
        SDL_Color bgColor = backgroundColors[currentState];
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderClear(renderer); 
        
        //Update and render current state
        currentStateInstance->update();

        if (currentStateInstance) {
            currentStateInstance->render(); }

        // Present renderer
        SDL_RenderPresent(renderer);
    }
// PQfinish(dbConn);

    TTF_CloseFont(NunitoFont);
    TTF_CloseFont(NimbusRomFont);
    TTF_CloseFont( middlesizeNimbusRomFont);
    TTF_CloseFont( BrilliantDreamsFont);
    TTF_CloseFont( BrockScript);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
