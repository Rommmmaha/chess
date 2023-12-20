#include"classes.hpp"

int main()
{
    board main_board;
    int type{0};
    char title[]{"Chess game V1"};
    char keys[]{'0','1','2','3','4','5','6','7','8','9'};
    Vector2 selection{-1,-1},tmp{0,0};
    InitWindow(int(main_board.tile_size*main_board.board_size+main_board.tile_size),int(main_board.tile_size*main_board.board_size),title);
    SetConfigFlags(FLAG_VSYNC_HINT);
    char path[]{"skins\\0.png"};
    main_board.main_texture=LoadTexture(path);
    for(; !WindowShouldClose();)
    {
        BeginDrawing();
        {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)&&!main_board.end)
            {
                Vector2 mouse{GetMousePosition()};
                mouse.x=int(mouse.x/main_board.tile_size);
                mouse.y=int(mouse.y/main_board.tile_size);
                type=main_board.pieces_pos[int(mouse.y)][int(mouse.x)];
                Vector2 move{-1,-1};
                if(mouse.x>7) { main_board.draw_history=(main_board.draw_history+1)%2; }
                for(hint i:main_board.possible_moves) { if(mouse.x==i.pos.x&&mouse.y==i.pos.y) { move=i.pos; } }
                if(move.x!=-1) { main_board.move(selection,move); }
                else
                {
                    if(type!=0&&((type<10&&main_board.walking_now=='W')||(type>10&&main_board.walking_now=='B')))
                    {
                        selection=mouse;
                    }
                    else { selection={-1,-1}; }
                    main_board.calculate_hints(selection);
                }
            }
            for(char i:keys)
            {
                path[6]=i;
                if(IsKeyPressed(i)) { main_board.main_texture=LoadTexture(path); }
                if(main_board.main_texture.id<=0) { path[6]='0'; main_board.main_texture=LoadTexture(path); }
            }
            if(selection.x!=tmp.x||selection.y!=tmp.y) { tmp=selection; }
            main_board.draw_board();
            if(tmp.x!=-1) { main_board.draw_hints(tmp); }
            main_board.draw_pieces();
            if(main_board.end) { main_board.draw_end(); }
        }
        EndDrawing();
    }
}
