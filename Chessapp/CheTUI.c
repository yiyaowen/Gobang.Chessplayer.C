#include <stdio.h>
#include <string.h>
#include <stdbool.h> // bool

#include "CheData.h"
#include "CheDef.h"
#include "CheGlobal.h"
#include "CheTUI.h"
INCLUDE_DOUBLE_BUFFER

#include "Cor.h" // GetCoreAnalysisResultP

////////////////
// Global TUI //
////////////////

void InitRoute(Route *route)
{
    route->status = ROUTE_CONTINUE;
    route->exit_status = ROUTE_SUCCESS;
}

Route *StartRoutine(Page *pNewPage, Route *route)
{
    PushPage(pNewPage);
    route = HandlePage(route);
    PopPage();
    return route;
}

void PushPage(Page *pNewPage)
{
    if (PS.count < PAGE_STACK_CAPACITY) {
        PS.pTopPage = PS.pages[PS.count++] = pNewPage;
    }
    else {
        AbortWithMsg("Stack Overflow: PageStack holds at most 10 pages.");
    }
}

Route *HandlePage(Route *route)
{
    while (1) {
        autodisplay((*(PS.pTopPage->pfnDisplayer))(PS.pTopPage->page_info));
        route = (*(PS.pTopPage->pfnHandler))(route, PS.pTopPage->page_info);
        if (route->status == ROUTE_OVER) return route;
    }
}

void PopPage()
{
    if (PS.count > 0) {
        --PS.count;
        PS.pTopPage = PS.pages[max(0,PS.count-1)];
    }
    else {
        AbortWithMsg("Stack Underflow: PageStack holds at least 0 pages.");
    }
}

///////////////
// TUI Utils //
///////////////

int GetValidOption(int iBaseOption, int iOptionCount)
{
    char c;
    int iOption;

get_user_input:
    c = getchar();
    if (c == '\n') {
        PutCharBack(c);
        ClearInputBuffer();
        return OPTION_Confirm_NUM;
    }
    else {
        iOption = (int)(c - '0');
        if (iOption > 0 && iOption <= iOptionCount) {
            ClearInputBuffer();
            return iOption+iBaseOption*10;
        }
        else {
            autoprint(
                printf("No such option. Please choose from 1~%d, or press Enter.", iOptionCount);
                putchar('\n');
                printf(PROMPT);
            );
            ClearInputBuffer();
            goto get_user_input;
        }
    }
}

//////////////
// Home TUI //
//////////////

void InitHomePageInfo(HomePageInfo info)
{
    info->iOptionCount = HOME_OPTION_NUM;
    info->iOptionSelected = OPTION_PvP_NUM;
}

void InitHomePage(Page *home_page)
{
    HomePageInfo home_page_info = malloc(sizeof(HomePageInfoType));
    InitHomePageInfo(home_page_info);
    home_page->id = PAGE_ID_Home;
    home_page->name = PAGE_NAME_Home;
    home_page->desc = PAGE_DESC_Home;
    home_page->page_info = home_page_info;
    home_page->pfnHandler = HandleHomePage;
    home_page->pfnDisplayer = DisplayHomePage;
}

void DisplayHomePage(PageInfo page_info)
{
    HomePageInfo info = (HomePageInfo)page_info;

    printf(HOME_ICON);
    putchar('\n');
    putchar('\n');

    for (int i = 0; i < info->iOptionCount; ++i) {

        PrintSpaces(20);
        
        if (i+1 == info->iOptionSelected) {

            printf(BLUE_TEXT(HIGHLIGHT_TEXT(SELECT_ARROW)));
            PrintSpaces(2);

            if (i+1 == OPTION_Exit_NUM) {
                printf(RED_TEXT(HIGHLIGHT_TEXT(UNDERLINE_TEXT("%s"))), HomeOptions[i]);
            }
            else {
                printf(HIGHLIGHT_TEXT(UNDERLINE_TEXT("%s")), HomeOptions[i]);
            }
        } 
        else {

            PrintSpaces(5);
            printf("%s", HomeOptions[i]);
        }
        putchar('\n');
        putchar('\n');
    }

    printf(PROMPT);
}

Route *HandleHomePage(Route *route, PageInfo page_info)
{
    HomePageInfo info = (HomePageInfo)page_info;

    int iOption;
    
    iOption = GetValidOption(OPTION_Home_NUM, HOME_OPTION_NUM);
    
    if (iOption == OPTION_Confirm_NUM) {

        Route* sub_route = malloc(sizeof(Route));
        InitRoute(sub_route);

        switch (info->iOptionSelected)
        {
        case OPTION_PvP_NUM:
        {
            Page pvp_page;
            InitPvPPage(&pvp_page);
            sub_route = StartRoutine(&pvp_page, sub_route);
            break;
        }
            
        case OPTION_PvC_NUM:
        {
            Page pvc_page;
            InitPvCPage(&pvc_page);
            sub_route = StartRoutine(&pvc_page, sub_route);
            break;
        }

        case OPTION_PreAndSet_NUM:
        {
            Page pre_and_set_page;
            InitPreAndSetPage(&pre_and_set_page);
            sub_route = StartRoutine(&pre_and_set_page, sub_route);
            break;
        }

        case OPTION_AboutChe_NUM:
        {
            Page about_che_page;
            InitAboutChePage(&about_che_page);
            sub_route = StartRoutine(&about_che_page, sub_route);
            break;
        }

        case OPTION_AboutPro_NUM:
        {
            Page about_pro_page;
            InitAboutProPage(&about_pro_page);
            sub_route = StartRoutine(&about_pro_page, sub_route);
            break;
        }

        case OPTION_Exit_NUM:
        {
            route->status = ROUTE_OVER; 
            route->exit_status = ROUTE_SUCCESS;
            return route;
        }

        default:
            route->status = ROUTE_OVER;
            route->exit_status = ROUTE_FAILURE;
            return route;
        }
        if (sub_route->exit_status == ROUTE_FAILURE) {
            route->status = ROUTE_OVER;
            route->exit_status = ROUTE_FAILURE;
            return route;
        }
    }
    else {
        info->iOptionSelected = iOption;
        route->status = ROUTE_CONTINUE;
    }
    return route;
}

//////////////
// Game TUI //
//////////////

void InitGamePageInfo(GamePageInfo info)
{
    InitGPC(info->game_prefab_config);
}

void InitGamePage(Page *game_page)
{
    GamePageInfo game_page_info = malloc(sizeof(GamePageInfoType));
    InitGamePageInfo(game_page_info);
    game_page->id = PAGE_ID_PvP;
    game_page->name = PAGE_NAME_PvP;
    game_page->desc = PAGE_DESC_PvP;
    game_page->page_info = game_page_info;
    game_page->pfnHandler = HandleGamePage;
    game_page->pfnDisplayer = DisplayGamePage;
}
void InitPvPPage(Page *pvp_page)
{
    InitGamePage(pvp_page);
    ((GamePageInfo)pvp_page->page_info)->game_prefab_config.mode = GPC_MODE_PVP;
}
void InitPvCPage(Page *pvc_page)
{
    InitGamePage(pvc_page); 
    ((GamePageInfo)pvc_page->page_info)->game_prefab_config.mode = GPC_MODE_PVC;
}

void DisplayGamePage(PageInfo page_info)
{
    GamePageInfo info = (GamePageInfo)page_info;

    POSITION pos;
    InitPos(pos);
    DisplayBoard(pos);
}

void DisplayBoard(POSITION pos)
{
    int i, j;
    char row;
    char col;

    for(j = 0, row = 15; j <= BOARD_SIZE - 1; j++) {

        // Display left infoboard
    
        if (j == 0) {
            printf(GAME_INFO_FRAME_TOP);
        }
        else {
            printf(GAME_INFO_FRAME_MIDDLE);
        }
        PrintSpaces(2);

        // Display chessboard
    
        if (row == pos.x) {
            printf(RED_TEXT(HIGHLIGHT_TEXT("%2d ")), row);
        }
        else {
            printf("%2d ", row);
        }
        row -= 1;

        for (i = 0; i <= BOARD_SIZE - 1; i++) {

            switch(Board[j][i])
            {
            case GRID_LEFT_TOP:
                if (j == (BOARD_SIZE - pos.x) && i == (pos.y - 'A')) {
                    printf(RED_TEXT(HIGHLIGHT_TEXT(LEFT_TOP_ICON)));
                }
                else {
                    printf(LEFT_TOP_ICON);
                }
                break;
				
            case GRID_RIGHT_TOP:
                if (j == (BOARD_SIZE - pos.x) && i == (pos.y - 'A')) {
                    printf(RED_TEXT(HIGHLIGHT_TEXT(RIGHT_TOP_ICON)));
                }
                else {
                    printf(RIGHT_TOP_ICON);
                }
                break;
				
            case GRID_RIGHT_BOTTOM:
                if (j == (BOARD_SIZE - pos.x) && i == (pos.y - 'A')) {
                    printf(RED_TEXT(HIGHLIGHT_TEXT(RIGHT_BOTTOM_ICON)));
                }
                else {
                    printf(RIGHT_BOTTOM_ICON);
                }
				break;
				
            case GRID_LEFT_BOTTOM:
                if (j == (BOARD_SIZE - pos.x) && i == (pos.y - 'A')) {
                    printf(RED_TEXT(HIGHLIGHT_TEXT(LEFT_BOTTOM_ICON)));
                }
                else {
                    printf(LEFT_BOTTOM_ICON);
                }
                break;
				
            case GRID_LEFT:
                if (j == (BOARD_SIZE - pos.x) && i == (pos.y - 'A')) {
                    printf(RED_TEXT(HIGHLIGHT_TEXT(LEFT_ICON)));
                }
                else {
                    printf(LEFT_ICON);
                }
                break;
				    
                case GRID_TOP:
                if (j == (BOARD_SIZE - pos.x) && i == (pos.y - 'A')) {
                    printf(RED_TEXT(HIGHLIGHT_TEXT(TOP_ICON)));
                }
                else {
                    printf(TOP_ICON);
                }
                break;
				
			case GRID_RIGHT:
                if (j == (BOARD_SIZE - pos.x) && i == (pos.y - 'A')) {
                    printf(RED_TEXT(HIGHLIGHT_TEXT(RIGHT_ICON)));
                }
                else {
                    printf(RIGHT_ICON);
                }
                break;
				
            case GRID_BOTTOM:
                if (j == (BOARD_SIZE - pos.x) && i == (pos.y - 'A')) {
                    printf(RED_TEXT(HIGHLIGHT_TEXT(BOTTOM_ICON)));
                }
                else {
                    printf(BOTTOM_ICON);
                }
                break;
				
            case GRID_MIDDLE:
                if (j == (BOARD_SIZE - pos.x) && i == (pos.y - 'A')) {
                    printf(RED_TEXT(HIGHLIGHT_TEXT(MIDDLE_ICON)));
                }
                else if ((j == (BOARD_SIZE - 4) && i == ('D' - 'A')) ||
                         (j == (BOARD_SIZE - 4) && i == ('L' - 'A')) ||
                         (j == (BOARD_SIZE - 12) && i == ('D' - 'A')) ||
                         (j == (BOARD_SIZE - 12) && i == ('L' - 'A')) ||
                         (j == (BOARD_SIZE - 8) && i == ('H' - 'A')))
                {
                    printf(HIGHLIGHT_TEXT(MIDDLE_ICON));
                }
                else {
                    printf(MIDDLE_ICON);
                }
                break;
				
            case BLACK_TRI:      
                printf(BLACK_TRI_ICON);
                break;
				
            case WHITE_TRI:      
                printf(WHITE_TRI_ICON);
                break;
				
            case BLACK_CIR:      
                printf(BLACK_CIR_ICON);
                break;
            
            case WHITE_CIR:
                printf(WHITE_CIR_ICON);
                break;
            }
        }

        // Display right infoboard
    
        PrintSpaces(1);
        if (j == 0) {
            printf(GAME_INFO_FRAME_TOP);
        }
        else {
            printf(GAME_INFO_FRAME_MIDDLE);
        }

        putchar('\n');

    }

    // Display last row

    printf(GAME_INFO_FRAME_BOTTOM);
    PrintSpaces(2);
	
    printf("   ");
    for (col = 'A'; col < 'A' + BOARD_SIZE ; col++) {
        if (col == pos.y) {
            printf(RED_TEXT(HIGHLIGHT_TEXT("%c ")),col);
        }
        else {
            printf("%c ", col);
        }
    }

    PrintSpaces(1);
    printf(GAME_INFO_FRAME_BOTTOM);

    // Go to new line
	
    putchar('\n');
}

Route *HandleGamePage(Route *route, PageInfo page_info)
{
    GamePageInfo info = (GamePageInfo)page_info;
    switch (info->game_prefab_config.mode)
    {
    case GPC_MODE_PVP:
        route = StartPvP(route, info->game_prefab_config);
        break;

    case GPC_MODE_PVC:
        route = StartPvC(route, info->game_prefab_config);
        break;
    
    default:
        route->status = ROUTE_OVER;
        route->exit_status = ROUTE_FAILURE;
        break;
    }
    return route;
}

Route *StartPvP(Route *route, GAME_PREFAB_CONFIG game_prefab_config)
{
    int iTotalRound = 0;
    int iGameResult;
    POSITION pos;
    POSITION lastPos;

    InitBoardArray();
    InitRecordBoardArray();
    InitPos(pos);
    InitPos(lastPos);

    autodisplay(DisplayBoard(pos));

    while (1) 
    {
        ////////////////////
        /* Round of Black */
        ////////////////////
        do {

            pos = GetValidPosition(ROUND_BLACK, pos); 
            if (pos.status == POS_QUIT) {
                route->status = ROUTE_OVER;
                route->exit_status = ROUTE_SUCCESS;
                return route;
            }

            autodisplay(DisplayBoard(pos));

        } while (pos.status == POS_PENDING);

        ++iTotalRound;
        UpdateGlobalBoardArrays(iTotalRound, ROUND_BLACK, lastPos, pos);
        lastPos = pos;
        autodisplay(DisplayBoard(pos));

        iGameResult = GetGameResult(RECORD_BLACK);
        if (iGameResult == GR_WIN) {
            // TODO
            printf("BLACK WIN\n");
            exit(0);
        }

        ////////////////////
        /* Round of White */
        ////////////////////
        do {
        
            pos = GetValidPosition(ROUND_WHITE, pos);
            if (pos.status == POS_QUIT) {
                route->status = ROUTE_OVER;
                route->exit_status = ROUTE_SUCCESS;
                return route;
            }

            autodisplay(DisplayBoard(pos));

        } while (pos.status == POS_PENDING);

        ++iTotalRound;
        UpdateGlobalBoardArrays(iTotalRound, ROUND_WHITE, lastPos, pos);
        lastPos = pos;
        autodisplay(DisplayBoard(pos));

        iGameResult = GetGameResult(RECORD_WHITE);
        if (iGameResult == GR_WIN) {
            // TODO
            printf("WHITE WIN\n");
            exit(0);
        }
    }   
    route->status = ROUTE_OVER;
    route->exit_status = ROUTE_FAILURE;
    return route;
}

Route *StartPvC(Route *route, GAME_PREFAB_CONFIG game_prefab_config)
{
    int iTotalRound = 0;
    int iGameResult;
    POSITION pos;
    POSITION lastPos;
    bool fPlayerFirst = (game_prefab_config.order == GPC_ORDER_PLAYER) ? true : false;

    InitBoardArray();
    InitRecordBoardArray();
    InitPos(pos);
    InitPos(lastPos);

    autodisplay(DisplayBoard(pos));

    while (1)
    {
        ////////////////////
        /* Round of Black */
        ////////////////////
        do {

            if (fPlayerFirst) {
                pos = GetValidPosition(ROUND_BLACK, pos);
            }
            else {
                ANALYSIS_PREFAB_CONFIG analysis_prefb_config = {
                    APC_SIDE_BLACK, // iSide
                    APC_LEVEL_DRUNK // iLevel
                };
                COR_POSITION cor_pos = GetCoreAnalysisResultP(RecordBoard, analysis_prefb_config);
                pos.x = cor_pos.x;
                pos.y = cor_pos.y;
                pos.status = POS_VERIFIED;
            }
            if (pos.status == POS_QUIT) {
                route->status = ROUTE_OVER;
                route->exit_status = ROUTE_FAILURE;
                return route;
            }

            autodisplay(DisplayBoard(pos));

        } while (pos.status == POS_PENDING);

        ++iTotalRound;
        UpdateGlobalBoardArrays(iTotalRound, ROUND_BLACK, lastPos, pos);
        lastPos = pos;
        autodisplay(DisplayBoard(pos));

        iGameResult = GetGameResult(RECORD_BLACK);
        if (iGameResult == GR_WIN) {
            // TODO
            printf("BLACK WIN\n");
            exit(0);
        }

        ////////////////////
        /* Round of White */
        ////////////////////
        do {
        
            if (!fPlayerFirst) {
                pos = GetValidPosition(ROUND_WHITE, pos);
            }
            else {
                ANALYSIS_PREFAB_CONFIG analysis_prefb_config = {
                    APC_SIDE_WHITE, // iSide
                    APC_LEVEL_DRUNK // iLevel
                };
                COR_POSITION cor_pos = GetCoreAnalysisResultP(RecordBoard, analysis_prefb_config);
                pos.x = cor_pos.x;
                pos.y = cor_pos.y;
                pos.status = POS_VERIFIED;
            }
            if (pos.status == POS_QUIT) {
                route->status = ROUTE_OVER;
                route->exit_status = ROUTE_FAILURE;
                return route;
            }

            autodisplay(DisplayBoard(pos));

        } while (pos.status == POS_PENDING);

        ++iTotalRound;
        UpdateGlobalBoardArrays(iTotalRound, ROUND_WHITE, lastPos, pos);
        lastPos = pos;
        autodisplay(DisplayBoard(pos));

        iGameResult = GetGameResult(RECORD_WHITE);
        if (iGameResult == GR_WIN) {
            // TODO
            printf("WHITE WIN\n");
            exit(0);
        }
    }
    route->status = ROUTE_OVER;
    route->exit_status = ROUTE_FAILURE;
    return route;
}

void PrintHint(int iRound)
{
    if (iRound == ROUND_BLACK) {
        printf("Black " BLACK_CIR_ICON "'s turn. ");
    }
    else if (iRound == ROUND_WHITE) {
        printf("White " WHITE_CIR_ICON "'s turn. ");
    }
    printf("Please input the position, e.g. 'A4'.");
    putchar('\n');
    printf(PROMPT);
}

POSITION GetValidPosition(int iRound, POSITION pos)
{
    char c;
    int iErrorType;
    int iScanResult;
    POSITION valid_pos = pos;

    autoprint(PrintHint(iRound));

get_user_input:
    c = getchar();
    if (IsSpecialControlOption(c)) {
        HandleControlOption(c, &valid_pos);
        return valid_pos;
    }
    else if (c == '\n') {
        PutCharBack(c);
        if (IsPositionValid(valid_pos) != POS_OVERLAPPED) {
            valid_pos.status = POS_VERIFIED;
            ClearInputBuffer();
            return valid_pos;
        }
        pos = valid_pos;
        iScanResult = 1;
    }
    else {
        pos.y = (c & 0x5f);
        iScanResult = scanf("%d", &pos.x);
        pos.status = POS_PENDING;
    }
    ClearInputBuffer();

    if ((iErrorType = IsPositionValid(pos)) != POS_VALID || iScanResult != 1) {
        if (iScanResult != 1) {
            iErrorType = POS_BAD_FORMAT;
        }
        autoprint(PrintErrorHint(iErrorType));
        goto get_user_input;
    }
    else {
        valid_pos = pos;
    }

    return valid_pos;
}

int IsSpecialControlOption(char key)
{
    if (key == 'q' || key == 'Q' ||
        key == 's' || key == 'S' ||
        key == 'v' || key == 'V') {
        return 1;
    }
    else {
        return 0;
    }
}

void HandleControlOption(char key, POSITION *inPos)
{
    switch (key)
    {
    // Quit
    case 'q':
    case 'Q':
        inPos->status = POS_QUIT;
        ClearInputBuffer();
        return;

    // Save
    case 's':
    case 'S':
    { // Need brace here, otherwise compile failed with [clang-1200.0.32.27]
        char path[80];
        int iReadPathResult;

        inPos->status = POS_PENDING;

        autoprint(
            printf("Save game record to file. Please input target file path.");
            putchar('\n');
            printf("File path: ");
        );

        iReadPathResult = scanf("%s", path); 
        ClearInputBuffer();
        if (iReadPathResult != 1) {
            autoprint(
                printf(HIGHLIGHT_TEXT(RED_TEXT("Can't read target file path."))
                    " Press enter to back to game. " HIGHLIGHT_TEXT("[Enter]"));
            );
            ClearInputBuffer();
            return;
        }

        if (!ExportBoardToFile(RecordBoard, path)) {
            autoprint(
                printf("Invalid target file path. Please make sure recursive directories exist.");
                putchar('\n');
                printf("Target file will be created if not exists. Try to check related permissions.");
                putchar('\n');
                printf(HIGHLIGHT_TEXT(RED_TEXT("Can't write target file."))
                       " Press enter to back to game." HIGHLIGHT_TEXT("[Enter]"));
            );
            ClearInputBuffer();
            return;
        }

        autoprint(
            printf("Target file %s saved successfully. Press enter to continue. "
               HIGHLIGHT_TEXT("[Enter]"), path);
        );
        ClearInputBuffer();
        return;
    }
        
    default:
        return;
    }
}

int IsPositionValid(POSITION pos)
{
    if (!('A' <= pos.y && pos.y <= 'O' && 1 <= pos.x && pos.x <= 15)) {
        return POS_OUT_OF_BOARD;
    }
    else if (Board[BOARD_SIZE-pos.x][pos.y-'A'] == BLACK_CIR ||
             Board[BOARD_SIZE-pos.x][pos.y-'A'] == BLACK_TRI ||
             Board[BOARD_SIZE-pos.x][pos.y-'A'] == WHITE_CIR ||
             Board[BOARD_SIZE-pos.x][pos.y-'A'] == WHITE_TRI) {
        return POS_OVERLAPPED;
    } 
    else {
        return POS_VALID;
    }
}
    
void PrintErrorHint(int iErrorType)
{
    switch (iErrorType)
    {
    case POS_OUT_OF_BOARD:
        printf("Position is out of the chessboard. Valid range: A~O & 1~15");
        putchar('\n');
        break;

    case POS_OVERLAPPED:
        printf("The position is occupied. Please choose another position.");
        putchar('\n');
        break;

    case POS_BAD_FORMAT:
        printf("Bad position format. Please input a letter followed by a number.");
        putchar('\n');
        break;

    default:
        printf("An unknown error occurred.");
        putchar('\n');
        break;
    }
    printf(PROMPT);
}

///////////////////
// PreAndSet TUI //
///////////////////

void InitPreAndSetPageInfo(PreAndSetPageInfo info)
{
    info->iOptionCount = OPTION_PreAndSet_NUM;
    info->iOptionSelected = OPTION_PreAndSet_Quit_NUM;
}

void InitPreAndSetPage(Page *pre_and_set_page)
{
    PreAndSetPageInfo pre_and_set_page_info = malloc(sizeof(PreAndSetPageInfoType));
    InitPreAndSetPageInfo(pre_and_set_page_info);
    pre_and_set_page->id = PAGE_ID_PreAndSet;
    pre_and_set_page->name = PAGE_NAME_PreAndSet;
    pre_and_set_page->desc = PAGE_DESC_PreAndSet;
    pre_and_set_page->page_info = pre_and_set_page_info;
    pre_and_set_page->pfnHandler = HandlePreAndSetPage;
    pre_and_set_page->pfnDisplayer = DisplayPreAndSetPage;
}

void DisplayPreAndSetPage(PageInfo page_info)
{
    PreAndSetPageInfo info = (PreAndSetPageInfo)page_info;

    printf(HOME_ICON);
    putchar('\n');
    putchar('\n');

    PrintPreAndSetOptions(info->iOptionSelected);

    printf(PROMPT);
}

void PrintPreAndSetOptions(int iOptionSelected)
{
    PrintPreAndSetOption(iOptionSelected, OPTION_PreAndSet_Quit_NUM, PREANDSET_OPTION_Quit);
    PrintPreAndSetOption(iOptionSelected, OPTION_PreAndSet_MusicSound_NUM, PREANDSET_OPTION_MusicSound);
    PrintPreAndSetOption(iOptionSelected, OPTION_PreAndSet_NetworkConfig_NUM, PREANDSET_OPTION_NetworkConfig);
}

void PrintPreAndSetOption(int iOptionSelected, int iOption, const char *szOptionText)
{
    PrintSpaces(20);
    if (iOptionSelected == iOption) {
        printf(BLUE_TEXT(HIGHLIGHT_TEXT(SELECT_ARROW)));
        PrintSpaces(2);
        printf(HIGHLIGHT_TEXT(UNDERLINE_TEXT("%s")), szOptionText);
    }
    else {
        PrintSpaces(5);
        printf("%s", szOptionText);
    }
    putchar('\n');
    putchar('\n');
}

Route *HandlePreAndSetPage(Route *route, PageInfo page_info)
{
    PreAndSetPageInfo info = (PreAndSetPageInfo)page_info;

    int iOption;     

    iOption = GetValidOption(OPTION_PreAndSet_NUM, PREANDSET_OPTION_NUM);

    if (iOption == OPTION_Confirm_NUM) {

        Route* sub_route = malloc(sizeof(Route));
        InitRoute(sub_route);

        switch (info->iOptionSelected)
        {
        case OPTION_PreAndSet_MusicSound_NUM:
            // TODO
            route->status = ROUTE_CONTINUE;
            return route;
            
        case OPTION_PreAndSet_NetworkConfig_NUM:
            // TODO
            route->status = ROUTE_CONTINUE;
            return route;

        case OPTION_PreAndSet_Quit_NUM:
            route->status = ROUTE_OVER;
            route->exit_status = ROUTE_SUCCESS;
            return route;

        default:
            route->status = ROUTE_OVER;
            route->exit_status = ROUTE_FAILURE;
            return route;
        }
        if (sub_route->exit_status == ROUTE_FAILURE) {
            route->status = ROUTE_OVER;
            route->exit_status = ROUTE_FAILURE;
        }
    }
    else {
        info->iOptionSelected = iOption;
        route->status = ROUTE_CONTINUE;
    }
    return route;
}

//////////////////
// AboutChe TUI //
//////////////////

void InitAboutChePageInfo(AboutChePageInfo info)
{
    // Reserved
}

void InitAboutChePage(Page *about_che_page)
{
    AboutChePageInfo about_che_page_info = malloc(sizeof(AboutChePageInfoType));
    InitAboutChePageInfo(about_che_page_info);
    about_che_page->id = PAGE_ID_AboutChe;
    about_che_page->name = PAGE_NAME_AboutChe;
    about_che_page->desc = PAGE_DESC_AboutChe;
    about_che_page->page_info = about_che_page_info;
    about_che_page->pfnHandler = HandleAboutChePage;
    about_che_page->pfnDisplayer = DisplayAboutChePage;
}

void DisplayAboutChePage(PageInfo page_info)
{
    AboutChePageInfo info = (AboutChePageInfo)page_info;

    printf(HOME_ICON);
    putchar('\n');
    putchar('\n');

    PrintAboutCheContent();
    putchar('\n');

    printf(PROMPT);
}

Route *HandleAboutChePage(Route *route, PageInfo page_info)
{
    AboutChePageInfo info = (AboutChePageInfo)page_info;

    char c;
    
get_user_input:
    if ((c = getchar()) != '\n') {
        ClearInputBuffer();
    }

    if (c == 'q' || c == 'Q') {
        route->status = ROUTE_OVER;
        route->exit_status = ROUTE_SUCCESS;
        return route;
    }
    else {
        autoprint(
            printf("Enter 'q' or 'Q' to quit.");
            putchar('\n');
            printf(PROMPT);
        );
        goto get_user_input;
    }
}

void PrintAboutCheContent()
{
    printf(ABOUT_CHESSPLAYER_CONTENT);
}

//////////////////
// AboutPro TUI //
//////////////////

void InitAboutProPageInfo(AboutProPageInfo info)
{
    info->iOptionSelected = OPTION_AboutPro_Normal_NUM;
}

void InitAboutProPage(Page *about_pro_page)
{
    AboutProPageInfo about_pro_page_info = malloc(sizeof(AboutProPageInfoType));
    InitAboutProPageInfo(about_pro_page_info);
    about_pro_page->id = PAGE_ID_AboutPro;
    about_pro_page->name = PAGE_NAME_AboutPro;
    about_pro_page->desc = PAGE_DESC_AboutPro;
    about_pro_page->page_info = about_pro_page_info;
    about_pro_page->pfnHandler = HandleAboutProPage;
    about_pro_page->pfnDisplayer = DisplayAboutProPage;
}

void DisplayAboutProPage(PageInfo page_info)
{
    AboutProPageInfo info = (AboutProPageInfo)page_info;
    
    printf(HOME_ICON);
    putchar('\n');
    putchar('\n');

    switch (info->iOptionSelected)
    {
    case OPTION_AboutPro_Normal_NUM:
        PrintAboutProContent();
        putchar('\n');
        break;
    
    case OPTION_AboutPro_EasterEgg1_NUM:
        PrintAboutProEasterEgg1Content();
        putchar('\n');
        break;

    case OPTION_AboutPro_EasterEgg2_NUM:
        PrintAboutProEasterEgg2Content();
        putchar('\n');
        break;

    default:
        printf("Unknown option for showing content of this page.");
        putchar('\n');
        break;
    }

    printf(PROMPT);
}

Route *HandleAboutProPage(Route *route, PageInfo page_info)
{
    AboutProPageInfo info = (AboutProPageInfo)page_info;

    info->iOptionSelected = GetValidAboutProOption();

    switch (info->iOptionSelected)
    {
    case OPTION_AboutPro_Quit_NUM:
        route->status = ROUTE_OVER;
        route->exit_status = ROUTE_SUCCESS;
        return route;

    case OPTION_AboutPro_EasterEgg1_NUM:
        route->status = ROUTE_CONTINUE;
        return route;

    case OPTION_AboutPro_EasterEgg2_NUM:
        route->status = ROUTE_CONTINUE;
        return route;

    default:
        route->status = ROUTE_OVER;
        route->exit_status = ROUTE_FAILURE;
        return route;
    }
}

void PrintAboutProContent()
{
    printf(ABOUT_PROJECT_CONTENT);
}

void PrintAboutProEasterEgg1Content()
{
    printf(ABOUT_PROJECT_EASTER_EGG_1_CONTENT);
}

void PrintAboutProEasterEgg2Content()
{
    printf(ABOUT_PROJECT_EASTER_EGG_2_CONTENT);
}

int GetValidAboutProOption()
{
    char c;
    
get_user_input:
    if ((c = getchar()) != '\n') {
        ClearInputBuffer();
    }
    switch (c)
    {
    // Quit
    case 'q':
    case 'Q':
        return OPTION_AboutPro_Quit_NUM;
    
    // Egg1
    case 'c':
    case 'C':
        return OPTION_AboutPro_EasterEgg1_NUM;

    // Egg2
    case 't':
    case 'T':
        return OPTION_AboutPro_EasterEgg2_NUM;

    default:
        autoprint(
            printf("Enter 'q' or 'Q' to quit, though you can also enter 'a' or 'B'.");
            putchar('\n');
            printf(PROMPT);
        );
        goto get_user_input;
    }
}
