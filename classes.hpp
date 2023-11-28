#include<raylib.h>
#include<vector>
#include<iostream>
#include<algorithm>
using namespace std;
void swap(int& a, int& b) { int c = a;	a = b;	b = c; }
Color colors[] = { {0,0,0,0},{200,200,200,150},{200,0,0,200} };
class hint
{
public:
    Vector2 pos;
    Color c;
};
class a2b
{
public:
    int type;
    char move[4];
    a2b(int _type, char _move[4])
    {
        type = _type;
        for (int i = 4; i--;) { move[i] = _move[i]; }
    }
    char* text()
    {
        char result[] = { move[0],move[1],'\n',move[2],move[3], 0 };
        return result;
    }
};
class board
{
public:
    const float tile_size{ 64 }, text_size{ 20 }, piece_size{ 64 }, board_size{ 8 };
    char walking_now{ 'W' }, end{ 0 };
    bool draw_history = 0;
    vector<int> dead[2];
    vector<a2b> history;
    vector<hint> possible_moves;
    Texture main_texture;
    //01,02,03,04,05,06
    //11,12,13,14,15,16
    int castling[2][2]{ {1,1},{1,1} };
    int pieces_pos[8][8]
    {
        {12,13,14,15,16,14,13,12},
        {11,11,11,11,11,11,11,11},
        {00,00,00,00,00,00,00,00},
        {00,00,00,00,00,00,00,00},
        {00,00,00,00,00,00,00,00},
        {00,00,00,00,00,00,00,00},
        {01,01,01,01,01,01,01,01},
        {02,03,04,05,06,04,03,02}
    };
    bool white() const { return walking_now == 'W'; }
    int get_type(Vector2 pos)const { return pieces_pos[int(pos.y)][int(pos.x)]; }
    int check(Vector2 selection, bool can_kill = true, bool can_walk = true)const
    {
        if ((selection.x < 0 || selection.x >= board_size) || (selection.y < 0 || selection.y >= board_size)) { return 0; }
        int type = get_type(selection);
        if (can_walk && type == 0) { return 1; }
        if (type != 0 && can_kill && ((type > 10 && white()) || (type < 10 && !white()))) { return 2; }
        return 0;
    };
    void draw_board()
    {
        ClearBackground(BLACK);
        for (int i = 0; i < board_size; i++)
        {
            for (int j = 0; j < board_size; j++)
            {
                DrawTexturePro(main_texture, { 0,(0 ? 1 : (i + j) % 2) * piece_size,piece_size,piece_size }, { j * tile_size,i * tile_size,tile_size,tile_size }, { 0,0 }, 0, WHITE);
            }
        }
        for (int i = 0; i < board_size * 2; i++)
        {
            DrawTexturePro(main_texture, { 0,(0 ? 1 : (i) % 2) * piece_size,piece_size,piece_size }, { board_size * tile_size,i * tile_size / 2,tile_size,tile_size / 2 }, { 0,0 }, 0, WHITE);
        }
        DrawRectangle(board_size * tile_size, 0, tile_size, board_size * tile_size, { 0,0,0,100 });
        draw_sideboard();
    };
    void draw_sideboard()
    {
        if (draw_history)
        {
            for (int i = 0; i < history.size(); i++)
            {
                int x = tile_size * board_size, y = tile_size * board_size;
                DrawTexturePro
                (
                    main_texture,
                    { (history[i].type % 10) * piece_size,(history[i].type > 10) * piece_size,piece_size,piece_size },
                    { float(x),float(y - (history.size() - i) * 32),32,32 }, { 0,0 }, 0, WHITE
                );
                DrawText(history[i].text(), x + 32, y - (history.size() - i) * 32 + 5, 10, WHITE);
            }
        }
        else
        {
            for (int i = 2; i--;)
            {
                for (int j = dead[i].size(); j--;)
                {
                    int x = tile_size * board_size, y = tile_size * board_size;
                    DrawTexturePro
                    (
                        main_texture,
                        { (dead[i][j] % 10) * piece_size,(0 ? 1 : dead[i][j] > 10) * piece_size,piece_size,piece_size},
                        { float(x) + 32 * i,float(y - (dead[i].size() - j) * 32),32,32}, {0,0}, 0, WHITE
                    );
                }
            }
        }
    }
    void draw_pieces()
    {
        for (int i = 0; i < board_size; i++)
        {
            for (int j = 0; j < board_size; j++)
            {
                if (pieces_pos[i][j] != 0)
                {
                    DrawTexturePro
                    (
                        main_texture,
                        { (pieces_pos[i][j] % 10) * piece_size,(0 ? 1 : pieces_pos[i][j] > 10) * piece_size,piece_size,piece_size },
                        { j * tile_size,i * tile_size,tile_size,tile_size }, { 0,0 }, 0, WHITE
                    );
                }
            }
        }
    };
    void calculate_hints(Vector2 selection)
    {
        int type = get_type(selection);
        possible_moves.clear();
        {
            if (pieces_pos[7][0] % 10 != 2) { castling[white()][0] = 0; }
            if (pieces_pos[7][7] % 10 != 2) { castling[white()][1] = 0; }
            if (pieces_pos[7][4] % 10 != 6) { castling[white()][0] = 0; castling[white()][1] = 0; }
        }
        if (type % 10 == 1 && selection.y != 0)
        {
            int tmp;
            tmp = check({ selection.x - 1,selection.y - 1 }, true, false);
            if (tmp) { possible_moves.push_back({ {selection.x - 1,selection.y - 1},colors[tmp] }); }
            tmp = check({ selection.x + 1,selection.y - 1 }, true, false);
            if (tmp) { possible_moves.push_back({ {selection.x + 1,selection.y - 1},colors[tmp] }); }
            tmp = check({ selection.x,selection.y - 1 }, false, true);
            if (tmp) { possible_moves.push_back({ {selection.x,selection.y - 1},colors[tmp] }); }
            if (selection.y == 6 && tmp)
            {
                tmp = check({ selection.x,selection.y - 2 }, false, true);
                if (tmp) { possible_moves.push_back({ {selection.x,selection.y - 2},colors[tmp] }); }
            }
        }
        if (type % 10 == 2 || type % 10 == 5)
        {
            int i;
            for (i = 1; check({ selection.x + i,selection.y }) != 0 && check({ selection.x + i,selection.y }) != 2; i++) { possible_moves.push_back({ {selection.x + i,selection.y},colors[1] }); }
            if (check({ selection.x + i,selection.y }) == 2) { possible_moves.push_back({ {selection.x + i,selection.y},colors[2] }); }
            for (i = 1; check({ selection.x - i,selection.y }) != 0 && check({ selection.x - i,selection.y }) != 2; i++) { possible_moves.push_back({ {selection.x - i,selection.y},colors[1] }); }
            if (check({ selection.x - i,selection.y }) == 2) { possible_moves.push_back({ {selection.x - i,selection.y},colors[2] }); }
            for (i = 1; check({ selection.x,selection.y + i }) != 0 && check({ selection.x,selection.y + i }) != 2; i++) { possible_moves.push_back({ {selection.x,selection.y + i},colors[1] }); }
            if (check({ selection.x,selection.y + i }) == 2) { possible_moves.push_back({ {selection.x,selection.y + i},colors[2] }); }
            for (i = 1; check({ selection.x,selection.y - i }) != 0 && check({ selection.x,selection.y - i }) != 2; i++) { possible_moves.push_back({ {selection.x,selection.y - i},colors[1] }); }
            if (check({ selection.x,selection.y - i }) == 2) { possible_moves.push_back({ {selection.x,selection.y - i},colors[2] }); }
        }
        if (type % 10 == 3)
        {
            vector<Vector2> moves
            {
                {-2,1},{2,-1},
                {-2,-1},{2,1},
                {-1,2},{1,-2},
                {-1,-2},{1,2}
            };
            for (auto i : moves)
            {
                int tmp{ check({selection.x + i.x,selection.y + i.y}) };
                if (tmp) { possible_moves.push_back({ {selection.x + i.x,selection.y + i.y}, colors[tmp] }); }
            }
        }
        if (type % 10 == 4 || type % 10 == 5)
        {
            int i;
            for (i = 1; check({ selection.x + i,selection.y + i }) != 0 && check({ selection.x + i,selection.y + i }) != 2; i++) { possible_moves.push_back({ {selection.x + i,selection.y + i},colors[1] }); }
            if (check({ selection.x + i,selection.y + i }) == 2) { possible_moves.push_back({ {selection.x + i,selection.y + i},colors[2] }); }
            for (i = 1; check({ selection.x - i,selection.y + i }) != 0 && check({ selection.x - i,selection.y + i }) != 2; i++) { possible_moves.push_back({ {selection.x - i,selection.y + i},colors[1] }); }
            if (check({ selection.x - i,selection.y + i }) == 2) { possible_moves.push_back({ {selection.x - i,selection.y + i},colors[2] }); }
            for (i = 1; check({ selection.x + i,selection.y - i }) != 0 && check({ selection.x + i,selection.y - i }) != 2; i++) { possible_moves.push_back({ {selection.x + i,selection.y - i},colors[1] }); }
            if (check({ selection.x + i,selection.y - i }) == 2) { possible_moves.push_back({ {selection.x + i,selection.y - i},colors[2] }); }
            for (i = 1; check({ selection.x - i,selection.y - i }) != 0 && check({ selection.x - i,selection.y - i }) != 2; i++) { possible_moves.push_back({ {selection.x - i,selection.y - i},colors[1] }); }
            if (check({ selection.x - i,selection.y - i }) == 2) { possible_moves.push_back({ {selection.x - i,selection.y - i},colors[2] }); }
        }
        if (type % 10 == 6)
        {
            vector<Vector2> moves
            {
                {-1,-1},{-1,0},{-1,1},
                {0,-1},{0,1},
                {1,-1},{1,0},{1,1}
            };
            for (auto i : moves)
            {
                int tmp{ check({selection.x + i.x,selection.y + i.y}) };
                if (tmp) { possible_moves.push_back({ {selection.x + i.x,selection.y + i.y}, colors[tmp] }); }
            }
            if (castling[white()][0])
            {
                if (check({ selection.x - 1,selection.y }) == 1 && check({ selection.x - 2,selection.y }) == 1 && check({ selection.x - 3,selection.y }) == 1)
                {
                    possible_moves.push_back({ {selection.x - 2,selection.y}, colors[1] });
                }
            }
            if (castling[white()][1])
            {
                if (check({ selection.x + 1,selection.y }) == 1 && check({ selection.x + 2,selection.y }) == 1)
                {
                    possible_moves.push_back({ {selection.x + 2,selection.y}, colors[1] });
                }
            }
        }
    };
    void move(Vector2 selection, Vector2 move)
    {
        char temp[4]{ 0,0,0,0 };
        temp[0] = 'a' + int(selection.x);
        temp[1] = '0' + (8 - int(selection.y));
        temp[2] = 'a' + int(move.x);
        temp[3] = '0' + (8 - int(move.y));
        if (!white())
        {
            temp[1] = '1' + int(selection.y);
            temp[3] = '1' + int(move.y);
        }
        history.push_back(a2b(pieces_pos[int(selection.y)][int(selection.x)], temp));

        if (get_type(move) != 0) { dead[white()].push_back(get_type(move)); sort(dead[white()].begin(), dead[white()].end()); }
        if (get_type(move) % 10 == 6) { end = walking_now; }
        swap(pieces_pos[int(selection.y)][int(selection.x)], pieces_pos[int(move.y)][int(move.x)]);
        if (move.y == 0 && pieces_pos[0][int(move.x)] % 10 == 1) { pieces_pos[0][int(move.x)] += 4; }
        pieces_pos[int(selection.y)][int(selection.x)] = 0;
        if (get_type(move) % 10 == 6 && abs(selection.x - move.x) == 2)
        {
            if (move.x < selection.x) { swap(pieces_pos[7][3], pieces_pos[7][0]); }
            else { swap(pieces_pos[7][5], pieces_pos[7][7]); }
        }
        if (!IsKeyDown(KEY_SPACE)) { reverce(); }
        possible_moves.clear();
    }
    void draw_hints(Vector2 selection) { for (hint i : possible_moves) { DrawRectangleV({ i.pos.x * tile_size,i.pos.y * tile_size }, { tile_size,tile_size }, i.c); } };
    void draw_end()
    {
        if (end == 'W') { DrawText("WHITE WINS !", 10, int(tile_size * board_size / 2.0 - text_size), int(text_size) * 2, GREEN); }
        if (end == 'B') { DrawText("BLACK WINS !", 10, int(tile_size * board_size / 2.0 - text_size), int(text_size) * 2, GREEN); }
    }
    void reverce()
    {
        for (int i{ 0 }; i < board_size / 2; i++) { for (int j{ 0 }; j < board_size; j++) { swap(pieces_pos[i][j], pieces_pos[int(board_size) - i - 1][j]); } }
        walking_now = white() ? 'B' : 'W';
    };
};