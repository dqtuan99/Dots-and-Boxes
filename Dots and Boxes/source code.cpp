#include <iostream>
#include <sstream>
#include <ctime>
#include <SFML/Graphics.hpp>
#include "Menu.hpp"
#include "Gameboard.hpp"

using namespace sf;
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::stringstream;

bool hline[5][5] = {0};
bool vline[4][6] = {0};
int Box[4][5] = {0};
int box_turn[4][5] = {0};
bool isPlayer1Turn = true;
int count1 = 0, count2 = 0;
int u, v, p, q;
bool isHorizontal = true;
int count;
bool end;
bool hasAI = true;

struct POSITION{
    int x, y;
};

void CheckBox(int i, int j){
    Box[i][j] = hline[i][j] + hline[i + 1][j] + vline[i][j] + vline[i][j + 1];
    if (Box[i][j] == 4){
        ++count2;
        box_turn[i][j] = 2;
    }
}

void TakeLine(bool isHorizontal, int i, int j){
    if (isHorizontal){
        hline[i][j] = true;
        if (i > 0){
            ++Box[i - 1][j];
            CheckBox(i - 1, j);
        }
        if (i < 5){
            ++Box[i][j];
            CheckBox(i, j);
        }
    }
    else{
        vline[i][j] = true;
        if (j > 0){
            ++Box[i][j - 1];
            CheckBox(i, j - 1);
        }
        if (j < 4){
            ++Box[i][j];
            CheckBox(i, j);
        }
    }
}

void TakeFreeBox3(){
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 5; ++j){
            if (Box[i][j] == 3){
                if (!vline[i][j]){
                    if (j == 0 || Box[i][j - 1] != 2) TakeLine(false, i, j);
                }
                else if (!hline[i][j]){
                    if (i == 0 || Box[i - 1][j] != 2) TakeLine(true, i, j);
                }
                else if (!vline[i][j + 1]){
                    if (j == 4 || Box[i][j + 1] != 2) TakeLine(false, i, j + 1);
                }
                else{
                    if (i == 3 || Box[i + 1][j] != 2) TakeLine(true, i + 1, j);
                }
            }
        }
    }
}

bool isBox3(){
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 5; ++j){
            if (Box[i][j] == 3){
                u = i;
                v = j;
                return true;
            }
        }
    }
    return false;
}

bool safeHline(int i, int j){
    if (!hline[i][j]){
        if (i == 0){
            if (Box[i][j] < 2) return true;
        }
        else if (i == 4){
            if (Box[i - 1][j] < 2) return true;
        }
        else{
            if (Box[i][j] < 2 && Box[i - 1][j] < 2) return true;
        }
    }
    return false;
}

bool randHline(int i, int j){
    p = i;
    q = j;
    do{
        if (safeHline(p, q)) return true;
        else{
            ++q;
            if (q == 5){
                q = 0;
                ++p;
                if (p == 5) p = 0;
            }
        }
    }
    while (p != i || q != j);
    return false;
}

bool safeVline(int i, int j){
    if (!vline[i][j]){
        if (j == 0){
            if (Box[i][j] < 2) return true;
        }
        else if (j == 5){
            if (Box[i][j - 1] < 2) return true;
        }
        else{
            if (Box[i][j] < 2 && Box[i][j - 1] < 2) return true;
        }
    }
    return false;
}

bool randVline(int i, int j){
    p = i;
    q = j;
    do{
        if (safeVline(p, q)) return true;
        else{
            ++q;
            if (q == 6){
                q = 0;
                ++p;
                if (p == 4) p = 0;
            }
        }
    }
    while (p != i || q != j);
    return false;
}

bool foundSafeLine(){
    srand(time(NULL));
    if (rand() % 2 == 1) isHorizontal = true;
    else isHorizontal = false;
    int i = rand() % 4;
    int j = rand() % 5;
    if (isHorizontal){
        if (randHline(i, j)) return true;
        else{
            isHorizontal = false;
            if (randVline(i, j)) return true;
        }
    }
    else{
        if (randVline(i, j)) return true;
        else{
            isHorizontal = true;
            if (randHline(i, j)) return true;
        }
    }
    return false;
}

void TakeBox(int i, int j){
    if (!hline[i][j]) TakeLine(true, i, j);
    else if (!vline[i][j]) TakeLine(false, i, j);
    else if (!hline[i + 1][j]) TakeLine(true, i + 1, j);
    else TakeLine(false, i, j + 1);
}

void TakeAllBox3(){
    while (isBox3()) TakeBox(u, v);
}

void TakeChain(int i, int j){
    int count = 1;
    int skip = 0;
    int ii = i, jj = j;
    bool end = false;
    while (1){
        if (!hline[ii][jj] && skip != 2){
            skip = 4;
            if (Box[ii - 1][jj] == 2 && ii > 0){
                ++count;
                --ii;
            }
            else if (Box[ii - 1][jj] == 3 && ii > 0){
                ++count;
                end = true;
                break;
            }
            else break;
        }
        else if (!hline[ii + 1][jj] && skip != 4){
            skip = 2;
            if (Box[ii + 1][jj] == 2 && ii < 4){
                ++count;
                ++ii;
            }
            else if (Box[ii + 1][jj] == 3 && ii < 4){
                ++count;
                end = true;
                break;
            }
            else break;
        }
        else if (!vline[ii][jj] && skip != 1){
            skip = 3;
            if (Box[ii][jj - 1] == 2 && jj > 0){
                ++count;
                --jj;
            }
            else if (Box[ii][jj - 1] == 3 && jj > 0){
                ++count;
                end = true;
                break;
            }
            else break;
        }
        else if (!vline[ii][jj + 1] && skip != 3){
            skip = 1;
            if (Box[ii][jj + 1] == 2 && jj < 5){
                ++count;
                ++jj;
            }
            else if (Box[ii][jj + 1] == 3 && jj < 5){
                ++count;
                end = true;
                break;
            }
            else break;
        }
        else break;
    }
    cout << count << endl;
    if (count + count1 + count2 == 20){
        TakeAllBox3();
    }
    else{
        if (end) count -= 2;
        skip = 0;
        while (count > 0){
            if (skip != 1 && !vline[i][j]){
                if (count != 2) TakeLine(false, i, j);
                --count;
                skip = 3;
                --j;
            }
            else if (skip != 2 && !hline[i][j]){
                if (count != 2) TakeLine(true, i, j);
                --count;
                skip = 4;
                --i;
            }
            else if (skip != 3 && !vline[i][j + 1]){
                if (count != 2) TakeLine(false, i, j + 1);
                --count;
                skip = 1;
                ++j;
            }
            else if (skip != 4 && !hline[i + 1][j]){
                if (count != 2) TakeLine(true, i + 1, j);
                --count;
                skip = 2;
                ++i;
            }
        }
    }
}

bool Give1Box(){
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 5; ++j){
            if (Box[i][j] == 2){
                int num = 0;
                if (!hline[i][j]){
                    if (i < 1 || Box[i - 1][j] < 2) ++num;
                }
                isHorizontal = false;
                if (!vline[i][j]){
                    if (j < 1 || Box[i][j - 1] < 2) ++num;
                    if (num > 1){
                        p = i;
                        q = j;
                        return true;
                    }
                }
                if (!vline[i][j + 1]){
                    if (j == 4 || Box[i][j + 1] < 2) ++num;
                    if (num > 1){
                        p = i;
                        q = j + 1;
                        return true;
                    }
                }
                isHorizontal = true;
                if (!hline[i + 1][j]){
                    if (i == 3 || Box[i + 1][j] < 2) ++num;
                    if (num > 1){
                        p = i + 1;
                        q = j;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Left(int i, int j){
    if (!vline[i][j]){
        if (j < 1 || Box[i][j - 1] < 2) return true;
    }
    else if (!hline[i][j]){
        if (i < 1 || Box[i - 1][j] < 2) return true;
    }
    else{
        if (i == 3 || Box[i + 1][j] < 2) return true;
    }
    return false;
}

bool Right(int i, int j){
    if (!vline[i][j + 1]){
        if (j == 4 || Box[i][j + 1] < 2) return true;
    }
    else if (!hline[i][j]){
        if (i < 1 || Box[i - 1][j] < 2) return true;
    }
    else{
        if (i == 3 || Box[i + 1][j] < 2) return true;
    }
    return false;
}

bool Up(int i, int j){
    if (!hline[i][j]){
        if (i < 1 || Box[i - 1][j] < 2) return true;
    }
    else if (!vline[i][j]){
        if (j < 1 || Box[i][j - 1] < 2) return true;
    }
    else{
        if (j == 4 || Box[i][j + 1] < 2) return true;
    }
    return false;
}

bool Down(int i, int j){
    if (!hline[i + 1][j]){
        if (i == 3 || Box[i + 1][j] < 2) return true;
    }
    else if (!vline[i][j]){
        if (j < 1 || Box[i][j - 1] < 2) return true;
    }
    else{
        if (j == 4 || Box[i][j + 1] < 2) return true;
    }
    return false;
}

bool Give2Box(){
    isHorizontal = false;
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 4; ++j){
            if (Box[i][j] == 2 && Box[i][j + 1] == 2 && !vline[i][j + 1]){
                if (Left(i, j) && Right(i, j + 1)){
                    p = i;
                    q = j + 1;
                    return true;
                }
            }
        }
    }
    isHorizontal = true;
    for (int j = 0; j < 5; ++j){
        for (int i = 0; i < 3; ++i){
            if (Box[i][j] == 2 && Box[i + 1][j] == 2 && !hline[i + 1][j]){
                if (Up(i, j) && Down(i + 1, j)){
                    p = i + 1;
                    q = j;
                    return true;
                }
            }
        }
    }
    return false;
}

void MakeRandomMove(){
    for(int i = 0; i < 5; ++i){
        for(int j = 0; j < 5; ++j){
            if (!hline[i][j]){
                TakeLine(true, i, j);
                return;
            }
        }
    }
    for(int i = 0; i < 4; ++i){
        for(int j = 0; j < 6; ++j){
            if (!vline[i][j]){
                TakeLine(false, i, j);
                return;
            }
        }
    }

}

void AI_easy(){
    TakeFreeBox3();
    if (isBox3()){
        if (foundSafeLine()){
            TakeAllBox3();
            TakeLine(isHorizontal, p, q);
        }
        else{
            TakeAllBox3();
            MakeRandomMove();
        }
    }
    else if (foundSafeLine()) TakeLine(isHorizontal, p, q);
    else if (Give1Box()) TakeLine(isHorizontal, p, q);
    else if (Give2Box()) TakeLine(isHorizontal, p, q);
    else MakeRandomMove();
}
void AI_hard(){ //dang bi loi chua sua duoc
    TakeFreeBox3();
    if (isBox3()){
        if (foundSafeLine()){
            TakeAllBox3();
            TakeLine(isHorizontal, p, q);
        }
        else{
            TakeChain(u, v);
        }
    }
    else if (foundSafeLine()) TakeLine(isHorizontal, p, q);
    else if (Give1Box()) TakeLine(isHorizontal, p, q);
    else if (Give2Box()) TakeLine(isHorizontal, p, q);
    else MakeRandomMove();
}

int main(){
    RenderWindow window(VideoMode(1024, 724), "DOTS AND BOXES");

    Texture background;
    background.loadFromFile("1.jpg");
    Sprite back;
    back.setTexture(background);

    Texture x_image;
    x_image.loadFromFile("x.png");
    Sprite x_sprite(x_image);

    Texture o_image;
    o_image.loadFromFile("o.png");
    Sprite o_sprite(o_image);

    int y = 330;
    int x = 270;
    POSITION dot[5][6];
    for(int i = 0; i < 5; ++i){
        x = 270;
        for(int j = 0; j < 6; ++j){
            dot[i][j].x = x;
            dot[i][j].y = y;
            x += 80;
        }
        y += 80;
    }

    y = 333;
    x = 276;
    POSITION line_h[5][5];
    for(int i = 0; i < 5; ++i){
        x = 276;
        for(int j = 0; j < 5; ++j){
            line_h[i][j].x = x;
            line_h[i][j].y = y;
            x += 80;
        }
        y += 80;
    }

    y = 336;
    x = 272;
    POSITION line_v[4][6];
    for(int i = 0; i < 4; ++i){
        x = 272;
        for(int j = 0; j < 6; ++j){
            line_v[i][j].x = x;
            line_v[i][j].y = y;
            x += 80;
        }
        y += 80;
    }

    y = 332;
    x = 272;
    POSITION box[4][5];
    for(int i = 0; i < 4; ++i){
        x = 272;
        for(int j = 0; j < 5; ++j){
            box[i][j].x = x;
            box[i][j].y = y;
            x += 80;
        }
        y += 80;
    }
    int m = -1, n = -1;
    int m2 = -1, n2 = -1;

    Menu menu(1024, 724);

    while (window.isOpen()){
		Event event;

		while (window.pollEvent(event)){
			switch (event.type){
            case Event::MouseButtonPressed:
			    if(event.mouseButton.x >= 300 && event.mouseButton.x <= 600){
			        if(event.mouseButton.y >= 530 && event.mouseButton.y <= 600)
                        window.close();
                    else if ( (event.mouseButton.y >= 330 && event.mouseButton.y <= 400)
                            ||(event.mouseButton.y >= 430 && event.mouseButton.y <= 500)){
                        if(event.mouseButton.y >= 330 && event.mouseButton.y <= 400)
                            hasAI = false;
                        if(event.mouseButton.y >= 430 && event.mouseButton.y <= 500)
                            hasAI = true;
                        while (window.isOpen()){
                            Event event;
                            while (window.pollEvent(event)){
                                if (event.type == Event::Closed){
                                    window.close();
                                }

                                window.clear();
                                window.draw(back);
                                bool hasEvent = false;

                                if (isPlayer1Turn == false && hasAI){
                                    AI_easy();
                                    isPlayer1Turn = !isPlayer1Turn;
                                }

                                if (event.type == Event::KeyPressed){
                                    if (event.key.code == Keyboard::F2 && (count1 + count2 == 20)){
                                        for(int i = 0; i < 4; ++i){
                                            for(int j = 0; j < 5; ++j){
                                                Box[i][j] = 0;
                                                hline[i][j] = 0;
                                                vline[i][j] = 0;
                                                box_turn[i][j] = 0;
                                            }
                                        }
                                        for (int j = 0; j < 5; ++j){
                                            hline[4][j] = 0;
                                        }
                                        for (int i = 0; i < 4; ++i){
                                            vline[i][5] = 0;
                                        }
                                        isPlayer1Turn = true;
                                        count1 = 0;
                                        count2 = 0;
                                    }
                                }

                                if (event.type == Event::MouseMoved){
                                    //Drawing horizontal lines
                                    if (event.mouseMove.y >= 323 && event.mouseMove.y <= 343){
                                        if (event.mouseMove.x >= 286 && event.mouseMove.x <= 340){
                                            m = 0;
                                            n = 0;
                                        }
                                        else if (event.mouseMove.x >= 366 && event.mouseMove.x <= 420){
                                            m = 0;
                                            n = 1;
                                        }
                                        else if (event.mouseMove.x >= 446 && event.mouseMove.x <= 500){
                                            m = 0;
                                            n = 2;
                                        }
                                        else if (event.mouseMove.x >= 526 && event.mouseMove.x <= 580){
                                            m = 0;
                                            n = 3;
                                        }
                                        else if (event.mouseMove.x >= 606 && event.mouseMove.x <= 660){
                                            m = 0;
                                            n = 4;
                                        }
                                    }

                                    else if (event.mouseMove.y >= 403 && event.mouseMove.y <= 423){
                                        if (event.mouseMove.x >= 286 && event.mouseMove.x <= 340){
                                            m = 1;
                                            n = 0;
                                        }
                                        else if (event.mouseMove.x >= 366 && event.mouseMove.x <= 420){
                                            m = 1;
                                            n = 1;
                                        }
                                        else if (event.mouseMove.x >= 446 && event.mouseMove.x <= 500){
                                            m = 1;
                                            n = 2;
                                        }
                                        else if (event.mouseMove.x >= 526 && event.mouseMove.x <= 580){
                                            m = 1;
                                            n = 3;
                                        }
                                        else if (event.mouseMove.x >= 606 && event.mouseMove.x <= 660){
                                            m = 1;
                                            n = 4;
                                        }
                                    }

                                    else if (event.mouseMove.y >= 483 && event.mouseMove.y <= 503){
                                        if (event.mouseMove.x >= 286 && event.mouseMove.x <= 340){
                                            m = 2;
                                            n = 0;
                                        }
                                        else if (event.mouseMove.x >= 366 && event.mouseMove.x <= 420){
                                            m = 2;
                                            n = 1;
                                        }
                                        else if (event.mouseMove.x >= 446 && event.mouseMove.x <= 500){
                                            m = 2;
                                            n = 2;
                                        }
                                        else if (event.mouseMove.x >= 526 && event.mouseMove.x <= 580){
                                            m = 2;
                                            n = 3;
                                        }
                                        else if (event.mouseMove.x >= 606 && event.mouseMove.x <= 660){
                                            m = 2;
                                            n = 4;
                                        }
                                    }

                                    else if (event.mouseMove.y >= 563 && event.mouseMove.y <= 583){
                                        if (event.mouseMove.x >= 286 && event.mouseMove.x <= 340){
                                            m = 3;
                                            n = 0;
                                        }
                                        else if (event.mouseMove.x >= 366 && event.mouseMove.x <= 420){
                                            m = 3;
                                            n = 1;
                                        }
                                        else if (event.mouseMove.x >= 446 && event.mouseMove.x <= 500){
                                            m = 3;
                                            n = 2;
                                        }
                                        else if (event.mouseMove.x >= 526 && event.mouseMove.x <= 580){
                                            m = 3;
                                            n = 3;
                                        }
                                        else if (event.mouseMove.x >= 606 && event.mouseMove.x <= 660){
                                            m = 3;
                                            n = 4;
                                        }
                                    }

                                    else if (event.mouseMove.y >= 643 && event.mouseMove.y <= 663){
                                        if (event.mouseMove.x >= 286 && event.mouseMove.x <= 340){
                                            m = 4;
                                            n = 0;
                                        }
                                        else if (event.mouseMove.x >= 366 && event.mouseMove.x <= 420){
                                            m = 4;
                                            n = 1;
                                        }
                                        else if (event.mouseMove.x >= 446 && event.mouseMove.x <= 500){
                                            m = 4;
                                            n = 2;
                                        }
                                        else if (event.mouseMove.x >= 526 && event.mouseMove.x <= 580){
                                            m = 4;
                                            n = 3;
                                        }
                                        else if (event.mouseMove.x >= 606 && event.mouseMove.x <= 660){
                                            m = 4;
                                            n = 4;
                                        }
                                    }

                                    else{
                                        m = -1;
                                        n = -1;
                                    }

                                    //Drawing vertical lines
                                    if (event.mouseMove.x >= 262 && event.mouseMove.x <= 282){
                                        if (event.mouseMove.y >= 346 && event.mouseMove.y <= 400){
                                            m2 = 0;
                                            n2 = 0;
                                        }
                                        else if (event.mouseMove.y >= 426 && event.mouseMove.y <= 480){
                                            m2 = 1;
                                            n2 = 0;
                                        }
                                        else if (event.mouseMove.y >= 506 && event.mouseMove.y <= 560){
                                            m2 = 2;
                                            n2 = 0;
                                        }
                                        else if (event.mouseMove.y >= 589 && event.mouseMove.y <= 640){
                                            m2 = 3;
                                            n2 = 0;
                                        }
                                    }

                                    else if (event.mouseMove.x >= 342 && event.mouseMove.x <= 362){
                                        if (event.mouseMove.y >= 346 && event.mouseMove.y <= 400){
                                            m2 = 0;
                                            n2 = 1;
                                        }
                                        else if (event.mouseMove.y >= 426 && event.mouseMove.y <= 480){
                                            m2 = 1;
                                            n2 = 1;
                                        }
                                        else if (event.mouseMove.y >= 506 && event.mouseMove.y <= 560){
                                            m2 = 2;
                                            n2 = 1;
                                        }
                                        else if (event.mouseMove.y >= 589 && event.mouseMove.y <= 640){
                                            m2 = 3;
                                            n2 = 1;
                                        }
                                    }

                                    else if (event.mouseMove.x >= 422 && event.mouseMove.x <= 442){
                                        if (event.mouseMove.y >= 346 && event.mouseMove.y <= 400){
                                            m2 = 0;
                                            n2 = 2;
                                        }
                                        else if (event.mouseMove.y >= 426 && event.mouseMove.y <= 480){
                                            m2 = 1;
                                            n2 = 2;
                                        }
                                        else if (event.mouseMove.y >= 506 && event.mouseMove.y <= 560){
                                            m2 = 2;
                                            n2 = 2;
                                        }
                                        else if (event.mouseMove.y >= 589 && event.mouseMove.y <= 640){
                                            m2 = 3;
                                            n2 = 2;
                                        }
                                    }

                                    else if (event.mouseMove.x >= 502 && event.mouseMove.x <= 522){
                                        if (event.mouseMove.y >= 346 && event.mouseMove.y <= 400){
                                            m2 = 0;
                                            n2 = 3;
                                        }
                                        else if (event.mouseMove.y >= 426 && event.mouseMove.y <= 480){
                                            m2 = 1;
                                            n2 = 3;
                                        }
                                        else if (event.mouseMove.y >= 506 && event.mouseMove.y <= 560){
                                            m2 = 2;
                                            n2 = 3;
                                        }
                                        else if (event.mouseMove.y >= 589 && event.mouseMove.y <= 640){
                                            m2 = 3;
                                            n2 = 3;
                                        }
                                    }

                                    else if (event.mouseMove.x >= 582 && event.mouseMove.x <= 602){
                                        if (event.mouseMove.y >= 346 && event.mouseMove.y <= 400){
                                            m2 = 0;
                                            n2 = 4;
                                        }
                                        else if (event.mouseMove.y >= 426 && event.mouseMove.y <= 480){
                                            m2 = 1;
                                            n2 = 4;
                                        }
                                        else if (event.mouseMove.y >= 506 && event.mouseMove.y <= 560){
                                            m2 = 2;
                                            n2 = 4;
                                        }
                                        else if (event.mouseMove.y >= 589 && event.mouseMove.y <= 640){
                                            m2 = 3;
                                            n2 = 4;
                                        }
                                    }

                                    else if (event.mouseMove.x >= 662 && event.mouseMove.x <= 682){
                                        if (event.mouseMove.y >= 346 && event.mouseMove.y <= 400){
                                            m2 = 0;
                                            n2 = 5;
                                        }
                                        else if (event.mouseMove.y >= 426 && event.mouseMove.y <= 480){
                                            m2 = 1;
                                            n2 = 5;
                                        }
                                        else if (event.mouseMove.y >= 506 && event.mouseMove.y <= 560){
                                            m2 = 2;
                                            n2 = 5;
                                        }
                                        else if (event.mouseMove.y >= 589 && event.mouseMove.y <= 640){
                                            m2 = 3;
                                            n2 = 5;
                                        }
                                    }

                                    else{
                                        m2 = -1;
                                        n2 = -1;
                                    }
                                }

                                if (event.type == Event::MouseButtonPressed){
                                    if (event.mouseButton.button == Mouse::Left){
                                        hasEvent = true;
                                        //Drawing horizontal lines
                                        if (event.mouseButton.y >= 323 && event.mouseButton.y <= 343){
                                            if (event.mouseButton.x >= 286 && event.mouseButton.x <= 340 && hline[0][0] != true){
                                                hline[0][0] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 366 && event.mouseButton.x <= 420 && hline[0][1] != true){
                                                hline[0][1] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 446 && event.mouseButton.x <= 500 && hline[0][2] != true){
                                                hline[0][2] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 526 && event.mouseButton.x <= 580 && hline[0][3] != true){
                                                hline[0][3] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 606 && event.mouseButton.x <= 660 && hline[0][4] != true){
                                                hline[0][4] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                        }

                                        if (event.mouseButton.y >= 403 && event.mouseButton.y <= 423){
                                            if (event.mouseButton.x >= 286 && event.mouseButton.x <= 340 && hline[1][0] != true){
                                                hline[1][0] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 366 && event.mouseButton.x <= 420 && hline[1][1] != true){
                                                hline[1][1] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 446 && event.mouseButton.x <= 500 && hline[1][2] != true){
                                                hline[1][2] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 526 && event.mouseButton.x <= 580 && hline[1][3] != true){
                                                hline[1][3] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 606 && event.mouseButton.x <= 660 && hline[1][4] != true){
                                                hline[1][4] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                        }

                                        if (event.mouseButton.y >= 483 && event.mouseButton.y <= 503){
                                            if (event.mouseButton.x >= 286 && event.mouseButton.x <= 340 && hline[2][0] != true){
                                                hline[2][0] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 366 && event.mouseButton.x <= 420 && hline[2][1] != true){
                                                hline[2][1] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 446 && event.mouseButton.x <= 500 && hline[2][2] != true){
                                                hline[2][2] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 526 && event.mouseButton.x <= 580 && hline[2][3] != true){
                                                hline[2][3] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 606 && event.mouseButton.x <= 660 && hline[2][4] != true){
                                                hline[2][4] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                        }

                                        if (event.mouseButton.y >= 563 && event.mouseButton.y <= 583){
                                            if (event.mouseButton.x >= 286 && event.mouseButton.x <= 340 && hline[3][0] != true){
                                                hline[3][0] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 366 && event.mouseButton.x <= 420 && hline[3][1] != true){
                                                hline[3][1] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 446 && event.mouseButton.x <= 500 && hline[3][2] != true){
                                                hline[3][2] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 526 && event.mouseButton.x <= 580 && hline[3][3] != true){
                                                hline[3][3] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 606 && event.mouseButton.x <= 660 && hline[3][4] != true){
                                                hline[3][4] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                        }

                                        if (event.mouseButton.y >= 643 && event.mouseButton.y <= 663){
                                            if (event.mouseButton.x >= 286 && event.mouseButton.x <= 340 && hline[4][0] != true){
                                                hline[4][0] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 366 && event.mouseButton.x <= 420 && hline[4][1] != true){
                                                hline[4][1] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 446 && event.mouseButton.x <= 500 && hline[4][2] != true){
                                                hline[4][2] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 526 && event.mouseButton.x <= 580 && hline[4][3] != true){
                                                hline[4][3] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.x >= 606 && event.mouseButton.x <= 660 && hline[4][4] != true){
                                                hline[4][4] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                        }

                                        //Drawing vertical lines
                                        if (event.mouseButton.x >= 262 && event.mouseButton.x <= 282){
                                            if (event.mouseButton.y >= 346 && event.mouseButton.y <= 400 && vline[0][0] != true){
                                                vline[0][0] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 426 && event.mouseButton.y <= 480 && vline[1][0] != true){
                                                vline[1][0] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 506 && event.mouseButton.y <= 560 && vline[2][0] != true){
                                                vline[2][0] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 589 && event.mouseButton.y <= 640 && vline[3][0] != true){
                                                vline[3][0] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                        }

                                        if (event.mouseButton.x >= 342 && event.mouseButton.x <= 362){
                                            if (event.mouseButton.y >= 346 && event.mouseButton.y <= 400 && vline[0][1] != true){
                                                vline[0][1] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 426 && event.mouseButton.y <= 480 && vline[1][1] != true){
                                                vline[1][1] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 506 && event.mouseButton.y <= 560 && vline[2][1] != true){
                                                vline[2][1] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 589 && event.mouseButton.y <= 640 && vline[3][1] != true){
                                                vline[3][1] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                        }

                                        if (event.mouseButton.x >= 422 && event.mouseButton.x <= 442){
                                            if (event.mouseButton.y >= 346 && event.mouseButton.y <= 400 && vline[0][2] != true){
                                                vline[0][2] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 426 && event.mouseButton.y <= 480 && vline[1][2] != true){
                                                vline[1][2] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 506 && event.mouseButton.y <= 560 && vline[2][2] != true){
                                                vline[2][2] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 589 && event.mouseButton.y <= 640 && vline[3][2] != true){
                                                vline[3][2] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                        }

                                        if (event.mouseButton.x >= 502 && event.mouseButton.x <= 522){
                                            if (event.mouseButton.y >= 346 && event.mouseButton.y <= 400 && vline[0][3] != true){
                                                vline[0][3] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 426 && event.mouseButton.y <= 480 && vline[1][3] != true){
                                                vline[1][3] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 506 && event.mouseButton.y <= 560 && vline[2][3] != true){
                                                vline[2][3] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 589 && event.mouseButton.y <= 640 && vline[3][3] != true){
                                                vline[3][3] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                        }

                                        if (event.mouseButton.x >= 582 && event.mouseButton.x <= 602){
                                            if (event.mouseButton.y >= 346 && event.mouseButton.y <= 400 && vline[0][4] != true){
                                                vline[0][4] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 426 && event.mouseButton.y <= 480 && vline[1][4] != true){
                                                vline[1][4] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 506 && event.mouseButton.y <= 560 && vline[2][4] != true){
                                                vline[2][4] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 589 && event.mouseButton.y <= 640 && vline[3][4] != true){
                                                vline[3][4] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                        }

                                        if (event.mouseButton.x >= 662 && event.mouseButton.x <= 682){
                                            if (event.mouseButton.y >= 346 && event.mouseButton.y <= 400 && vline[0][5] != true){
                                                vline[0][5] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 426 && event.mouseButton.y <= 480 && vline[1][5] != true){
                                                vline[1][5] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 506 && event.mouseButton.y <= 560 && vline[2][5] != true){
                                                vline[2][5] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                            else if (event.mouseButton.y >= 589 && event.mouseButton.y <= 640 && vline[3][5] != true){
                                                vline[3][5] = true;
                                                isPlayer1Turn = !isPlayer1Turn;
                                            }
                                        }
                                    }
                                }

                                for(int i = 0; i < 5; ++i){
                                    for(int j = 0; j < 6; ++j){
                                        window.draw(drawCircle(dot[i][j].x, dot[i][j].y));
                                    }
                                }

                                if (m != -1 && n != -1){
                                    window.draw(drawLineHorizontal(line_h[m][n].x, line_h[m][n].y, 1));

                                }

                                if (m2 != -1 && n2 != -1){
                                    window.draw(drawLineVertical(line_v[m2][n2].x, line_v[m2][n2].y, 1));

                                }

                                for(int i = 0; i < 5; ++i){
                                    for(int j = 0; j < 5; ++j){
                                        if (hline[i][j]){
                                            window.draw(drawLineHorizontal(line_h[i][j].x, line_h[i][j].y, 0));
                                        }
                                    }
                                }

                                for(int i = 0; i < 4; ++i){
                                    for(int j = 0; j < 6; ++j){
                                        if (vline[i][j]){
                                            window.draw(drawLineVertical(line_v[i][j].x, line_v[i][j].y, 0));
                                        }
                                    }
                                }

                                for(int i = 0; i < 4; ++i){
                                    for(int j = 0; j < 5; ++j){
                                        Box[i][j] = hline[i][j] + hline[i + 1][j] + vline[i][j] + vline[i][j + 1];
                                    }
                                }

                                bool flag = false;
                                if (hasEvent){
                                    for(int i = 0; i < 4; ++i){
                                        for(int j = 0; j < 5; ++j){
                                            if (Box[i][j] == 4){
                                                if(box_turn[i][j] == 0){
                                                    if (!flag){
                                                        isPlayer1Turn = !isPlayer1Turn;
                                                    }
                                                    if(isPlayer1Turn){
                                                        box_turn[i][j] = 1;
                                                        ++count1;
                                                    }
                                                    else{
                                                        box_turn[i][j] = 2;
                                                        ++count2;
                                                    }
                                                    flag = true;
                                                }
                                            }
                                        }
                                    }
                                }

                                for(int i = 0; i < 4; ++i){
                                    for(int j = 0; j < 5; ++j){
                                        if (Box[i][j]){
                                            if(box_turn[i][j] == 1){
                                                x_sprite.setPosition(box[i][j].x, box[i][j].y);
                                                window.draw(x_sprite);
                                            }
                                            else if (box_turn[i][j] == 2){
                                                o_sprite.setPosition(box[i][j].x, box[i][j].y);
                                                window.draw(o_sprite);
                                            }
                                        }
                                    }
                                }

                                Font font;
                                font.loadFromFile("arial.ttf");

                                Text player1;
                                player1.setFont(font);
                                player1.setString("Player 1:");
                                player1.setColor(Color::Red);
                                player1.setCharacterSize(35);
                                player1.setPosition(860, 280);
                                window.draw(player1);

                                stringstream sscore1;
                                sscore1 << count1;
                                string str1;
                                if (count1 < 10){
                                    str1 = '0' + sscore1.str();
                                }
                                else{
                                    str1 = sscore1.str();
                                }
                                Text score1;
                                score1.setFont(font);
                                score1.setString(str1);
                                score1.setColor(Color::Red);
                                score1.setCharacterSize(35);
                                score1.setPosition(905, 340);
                                window.draw(score1);

                                Text player2;
                                player2.setFont(font);
                                player2.setString("Player 2:");
                                player2.setColor(Color::Blue);
                                player2.setCharacterSize(35);
                                player2.setPosition(860, 480);
                                window.draw(player2);

                                stringstream sscore2;
                                sscore2 << count2;
                                string str2;
                                if (count2 < 10){
                                    str2 = '0' + sscore2.str();
                                }
                                else{
                                    str2 = sscore2.str();
                                }
                                Text score2;
                                score2.setFont(font);
                                score2.setString(str2);
                                score2.setColor(Color::Blue);
                                score2.setCharacterSize(35);
                                score2.setPosition(905, 540);
                                window.draw(score2);

                                bool endflag = false;
                                if (count1 + count2 == 20){
                                    endflag = true;

                                    Text reset;
                                    reset.setFont(font);
                                    reset.setString("F2: Reset");
                                    reset.setColor(Color::Red);
                                    reset.setCharacterSize(27);
                                    reset.setPosition(12, 630);
                                    window.draw(reset);

                                    Text gameover;
                                    gameover.setFont(font);
                                    gameover.setString("GAME OVER");
                                    gameover.setColor(Color::Black);
                                    gameover.setCharacterSize(100);
                                    gameover.setPosition(207, 0);
                                    window.draw(gameover);

                                    Text winner;
                                    winner.setFont(font);
                                    winner.setString("WINNER");
                                    winner.setCharacterSize(40);
                                    if (count2 > count1){
                                        winner.setColor(Color::Blue);
                                        winner.setPosition(845, 430);
                                        window.draw(winner);
                                    }
                                    else if (count2 < count1){
                                        winner.setColor(Color::Red);
                                        winner.setPosition(845, 230);
                                        window.draw(winner);
                                    }
                                    else{
                                        Text draw1, draw2;
                                        draw1.setFont(font);
                                        draw1.setString("DRAW");
                                        draw1.setCharacterSize(40);
                                        draw1.setColor(Color::Blue);
                                        draw1.setPosition(865, 430);
                                        window.draw(draw1);

                                        draw2.setFont(font);
                                        draw2.setString("DRAW");
                                        draw2.setCharacterSize(40);
                                        draw2.setColor(Color::Red);
                                        draw2.setPosition(865, 230);
                                        window.draw(draw2);
                                    }
                                }

                                if (!endflag){
                                    CircleShape arrow1(15, 3);
                                    arrow1.setFillColor(Color::Red);
                                    arrow1.setRotation(90);
                                    arrow1.setPosition(853, 286);
                                    if (isPlayer1Turn){
                                        window.draw(arrow1);
                                    }

                                    CircleShape arrow2(15, 3);
                                    arrow2.setFillColor(Color::Blue);
                                    arrow2.setRotation(90);
                                    arrow2.setPosition(853, 486);
                                    if (!isPlayer1Turn){
                                        window.draw(arrow2);
                                    }
                                }

                                window.display();
                            }
                        }
                    }
                }
                break;

			case Event::Closed:
				window.close();
				break;
			}
		}

        menu.draw(window);
        window.display();
	}
}
