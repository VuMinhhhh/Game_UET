#include "Game.h"

bool Game::Init(const char* title)
{
    if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        cout << "Subsystems Initialised!..." << endl;
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        if(window)
        {
            cout << "Window created!" << endl;
        }
        renderer = SDL_CreateRenderer(window, -1, 0);
        if(renderer)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            cout << "Renderer created!" << endl;
        }
        if( !(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0))
            {
                cout << "Mixer initialised!" << endl;
            }
        isRunning = true;
        bool MenuQuit = false;
        Menu = LoadTexture("Assets/menu.png");
        while(!MenuQuit)
        {
            SDL_Rect startButtonRect = {385, 310, 230, 110};
            SDL_Point mousePosition;
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        MenuQuit = true;
                        isRunning = false;
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
                            if (SDL_PointInRect(&mousePosition, &startButtonRect)) {
                                MenuQuit = true;
                            }
                            cout << 1 << endl;
                        }
                        break;
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            MenuQuit = true;
                        }
                        break;
                }
            }

            SDL_RenderCopy(renderer, Menu, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
        SDL_DestroyTexture(Menu);
        manager = new LevelManager();
        manager->LoadLevel();

        //Load music
        gMusic = Mix_LoadMUS( "Assets/1.wav" );

        //Load sound effects
        gNewLevel = Mix_LoadWAV( "Assets/NewLevel.mp3" );
        gWallHit = Mix_LoadWAV( "Assets/WallHit.mp3" );
        gBoxHit = Mix_LoadWAV( "Assets/BoxHit.wav" );

        WallTexture = LoadTexture("Assets/wall.png");
        GroundTexture = LoadTexture("Assets/ground.png");
        BoxTexture = LoadTexture("Assets/box.png");
        GoalTexture = LoadTexture("Assets/goal.png");

        player = new Player(this);

        InitLevel();

        return true;

    }
    isRunning = false;
    return false;
}

void Game::GameLoop()
{
    while(isRunning)
    {
        HandleEvents();
        Update();
        Draw();
    }
}

void Game::HandleEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            isRunning = false;
        }
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_RIGHT:
                player->Move(1, 0);
                break;
            case SDLK_LEFT:
                player->Move(-1, 0);
                break;
            case SDLK_DOWN:
                player->Move(0, 1);
                break;
            case SDLK_UP:
                player->Move(0, -1);
                break;
            case SDLK_SPACE:
                DestroyBoxes();
                InitLevel();
                break;
            case SDLK_9:
                if( Mix_PlayingMusic() == 0 )
                {
                    Mix_PlayMusic( gMusic, -1 );
                }
                else
                {
                    if( Mix_PausedMusic() == 1 )
                    {
                        Mix_ResumeMusic();
                    }
                    else
                    {
                        Mix_PauseMusic();
                    }
                }
                break;
            case SDLK_0:
                Mix_HaltMusic();
                MusicCount = MusicCount % 7 + 1;
                string tmp = "Assets/" + to_string(MusicCount) + ".mp3";
                const char* path = tmp.c_str();
                gMusic = Mix_LoadMUS( path );
                Mix_PlayMusic( gMusic, -1 );
                break;

            }
        }
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    if(keystates[SDL_SCANCODE_ESCAPE])
    {
        isRunning = false;
    }
}

void Game::Update()
{

}

void Game::Draw()
{
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);

    for(int row = 0; row < TILE_ROWS; row++)
        {
            for(int col = 0; col < TILE_COLS; col++)
            {
                SDL_Rect destRect = {col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                if(manager->LevelMap[col][row] == 'x')
                {
                    SDL_RenderCopy(renderer, WallTexture, NULL, &destRect);
                }
                else if(manager->LevelMap[col][row] == 'g')
                {
                    SDL_RenderCopy(renderer, GoalTexture, NULL, &destRect);
                }
                else
                {
                    SDL_RenderCopy(renderer, GroundTexture, NULL, &destRect);
                }
            }
        }

    for(auto b : boxes)
    {
        SDL_RenderCopy(renderer, BoxTexture, NULL, b->GetRect());
    }

    player->Draw(renderer);

    SDL_RenderPresent(renderer);
}

void Game::clean()
{
    // Free music
    Mix_FreeChunk( gNewLevel );
    Mix_FreeChunk( gWallHit );
    Mix_FreeChunk( gBoxHit );
    Mix_FreeMusic( gMusic );

    // Free texture
    SDL_DestroyTexture(WallTexture);
    SDL_DestroyTexture(GroundTexture);
    SDL_DestroyTexture(BoxTexture);
    SDL_DestroyTexture(GoalTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    Mix_Quit();
}

SDL_Texture* Game::LoadTexture(const char* path)
{
    SDL_Surface* tempSurface = IMG_Load(path);
    if(tempSurface == NULL)
        cout << "Failed to load surface: " << IMG_GetError() << endl;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    if(texture == NULL)
        cout << "Failed to convert to texture" << SDL_GetError() << endl;
    SDL_FreeSurface(tempSurface);
    return texture;
}

bool Game::HitWall(int x, int y)
{
    if(manager->LevelMap[x][y] == 'x') {
        Mix_PlayChannel( -1, gWallHit, 0 );
        return true;
    }
    return false;
}



bool Game::CanPushBox(Box* box, int x, int y)
{
    if(HitWall(x, y))
        return false;

    for(auto b : boxes)
    {
        if(b == box)
            continue;
        else if(x == b->GetPos().x && y == b->GetPos().y) {
            Mix_PlayChannel( -1, gWallHit, 0 );
            return false;
        }
    }
    Mix_PlayChannel( -1, gBoxHit, 0 );
    return true;
}

bool Game::BoxUpdated(int moveX, int moveY, int pX, int pY)
{
    Box* BoxToPush = NULL;
    for(auto b : boxes)
    {
        if(pX == b->GetPos().x && pY == b->GetPos().y)
        {
            BoxToPush = b;
            break;
        }
    }
    if(BoxToPush != NULL)
    {
        int toPushX = pX + moveX;
        int toPushY = pY + moveY;
        if(CanPushBox(BoxToPush, toPushX, toPushY))
        {
            bool InGoal = HitGoal(toPushX, toPushY);
            if(InGoal)
                cout << toPushX << " " << toPushY << " " << "Goal Hit!" << endl;
            BoxToPush->Update(toPushX, toPushY, InGoal);
            if(ALlGoalsCompleted())
            {
                cout << "All goals completed!" << endl;
                GoToNextLevel();
                return false;
            }
        }
        else
            return false;
    }
    return true;
}

bool Game::HitGoal(int x, int y)
{
    return manager->LevelMap[x][y] == 'g';
}

bool Game::ALlGoalsCompleted()
{
    for(auto b : boxes)
    {
        if(!b->GetInGoal())
            return false;
    }
    Mix_PlayChannel( -1, gNewLevel, 0 );
    return true;
}

void Game::DestroyBoxes()
{
    for(auto &b : boxes)
        delete b;
    boxes.erase(boxes.begin(), boxes.end());
}

void Game::InitLevel()
{
    for(int row = 0; row < TILE_ROWS; row++)
    {
        for(int col = 0; col < TILE_COLS; col++)
        {
            if(manager->LevelMap[col][row] == 'p')
            {
                player->SetPos(col, row);
            }
            else if(manager->LevelMap[col][row] == 'b')
            {
                boxes.emplace_back(new Box(col, row));
            }
        }
    }
}

void Game::GoToNextLevel()
{
    DestroyBoxes();

    manager->UpdateLevel();
    manager->LoadLevel();

    InitLevel();
}

