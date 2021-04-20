#include "depend.hpp"

#include "ResourcePath.hpp"


#include "Data_structs.hpp"
#include "Board.hpp"
#include "Utility.hpp"
#include "Search.hpp"


// Global vars:
sf::Texture textures[13];
std::forward_list<sf::Sprite> sprites, promotion_sprites_white, promotion_sprites_black;
int incre8[8];
double s_timer;




void set_single_texture(int color, piece_type piece, sf::Sprite& sprite) {
    int addon;
    if (color == 0) {
        addon = 0;
    }
    else {
        addon = 6;
    }
    switch (piece) {
        case Bishop:
            sprite.setTexture(textures[0 + addon]);
            break;
        case King:
            sprite.setTexture(textures[1 + addon]);
            break;
        case Knight:
            sprite.setTexture(textures[2 + addon]);
            break;
        case Pawn:
            sprite.setTexture(textures[3 + addon]);
            break;
        case Queen:
            sprite.setTexture(textures[4 + addon]);
            break;
        case Rook:
            sprite.setTexture(textures[5 + addon]);
            break;
        case Empty:
            break;
    }
}





void draw_pieces(sf::RenderWindow* window) {
    for (std::forward_list<sf::Sprite>::iterator it = sprites.begin() ; it != sprites.end(); ++it) {
        window->draw(*it);
    }
}


void draw_promotion_pieces(sf::RenderWindow* window, int current_turn) {
    if (current_turn) {
        for (std::forward_list<sf::Sprite>::iterator it = promotion_sprites_black.begin() ; it != sprites.end(); ++it) {
            window->draw(*it);
        }
    }
    else {
        for (std::forward_list<sf::Sprite>::iterator it = promotion_sprites_white.begin() ; it != sprites.end(); ++it) {
            window->draw(*it);
        }
    }
}



sf::Sprite* locate_sprite_clicked(std::forward_list<sf::Sprite>& list, int x, int y) {
    for (std::forward_list<sf::Sprite>::iterator it = list.begin() ; it != list.end(); ++it) {
        if ((*it).getGlobalBounds().contains(x, y)) {
            return (&(*it));
        }
    }
    return NULL;
}



int locate_sprite_clicked_index(std::forward_list<sf::Sprite>& list, int x, int y, sf::Sprite* sprite) {
    int counter = 0;
    for (std::forward_list<sf::Sprite>::iterator it = list.begin() ; it != list.end(); ++it) {
        if ((*it).getGlobalBounds().contains(x, y) && (&(*it) != sprite)) {
            return counter;
        }
        counter++;
    }
    return -1;
}


Cords find_grid_bounds(int x, int y) {
    Cords c;
    if (!(0 <= x && x <= WIDTH && 0 <= y && y <= WIDTH)) {
        c.x = -1;
        c.y = -1;
    } else {
        c.x = (x * 8) / WIDTH;
        c.y = (y * 8) / WIDTH;
    }
    return c;
}




void load_textures() {
    
    std::string str("bknpqr");
    std::string::iterator it = str.begin();
    std::string str2;
    sf::Texture texture;
    
    for (int i = 0; i < 6; i++) {
        str2 = resourcePath() + "Chess_";
        
        texture.loadFromFile(str2.append(1, *it) + "lt60.png");
        texture.setSmooth(true);
        textures[i] = texture;
        
        str2 = resourcePath() + "Chess_";
        texture.loadFromFile(str2.append(1, *it) + "dt60.png");
        texture.setSmooth(true);
        textures[i+6] = texture;
        
        ++it;
    }
    
    sf::Texture blank;
    blank.create(60, 60);
    textures[12] = blank;
}

void set_single_promotion_texture(int color, int i, sf::Sprite& sprite) {
    piece_type piece;
    switch (i) {
        case 0:
            piece = Queen;
            break;
        case 1:
            piece = Rook;
            break;
        case 2:
            piece = Bishop;
            break;
        case 3:
            piece = Knight;
            break;
        default:
            std::cout << "Should not have been reached. " << std::endl;
            break;
    }
    set_single_texture(color, piece, sprite);
}

void set_promotional_sprites() {
    for (int c = 0; c < 2; c++) {
        for (int i = 0; i < 4; i++) {
            sf::Sprite sprite;
            set_single_promotion_texture(c, i, sprite);
            
            sprite.setOrigin(sf::Vector2f(30, 30));
            sprite.setPosition(i * WIDTH/8 + WIDTH/16 + WIDTH / 4, WIDTH / 2);
            sprite.setScale(sf::Vector2f(SCALE, SCALE));
            
            if (c == 0) {
                promotion_sprites_black.push_front(sprite);
            }
            else {
                promotion_sprites_white.push_front(sprite);
            }
        }
    }
}


void castle_sprite_handler(int castle_side_value, Move validated_move) {
    if (validated_move.type == Castle_Kingside) {
        sf::Sprite* temp_sprite = locate_sprite_clicked(sprites, 7 * WIDTH/8 + WIDTH/16 - OFFSET, castle_side_value * WIDTH/8 + WIDTH/16 - OFFSET);
        temp_sprite->setPosition(5 * WIDTH/8 + WIDTH/16 - OFFSET, castle_side_value * WIDTH/8 + WIDTH/16 - OFFSET);
    }
    else if (validated_move.type == Castle_Queenside) {
        sf::Sprite* temp_sprite = locate_sprite_clicked(sprites, 0 * WIDTH/8 + WIDTH/16 - OFFSET, castle_side_value * WIDTH/8 + WIDTH/16 - OFFSET);
        temp_sprite->setPosition(3 * WIDTH/8 + WIDTH/16 - OFFSET, castle_side_value * WIDTH/8 + WIDTH/16 - OFFSET);
    }
}



void en_passant_sprite_handler(int en_passant_side_value, Move validated_move) {
    int temp_index;
    if (validated_move.type == En_Passant) {
        temp_index = locate_sprite_clicked_index(sprites, validated_move.to_c.x * WIDTH/8 + WIDTH/16 - OFFSET, en_passant_side_value * WIDTH/8 + WIDTH/16 - OFFSET, NULL);
        if (temp_index != -1) {
            std::forward_list<sf::Sprite>::iterator en_p_it = sprites.before_begin();
            std::advance(en_p_it, temp_index);
            sprites.erase_after(en_p_it);
        }
    }
}


void normal_move_sprite_handler(Move validated_move, sf::Sprite* sprite_being_dragged) {
    int temp_index = locate_sprite_clicked_index(sprites, validated_move.to_c.x * WIDTH/8 + WIDTH/16 - OFFSET, validated_move.to_c.y * WIDTH/8 + WIDTH/16 - OFFSET, sprite_being_dragged);
    
//    std::cout << c.x << ' ' << c.y << std::endl;
    sprite_being_dragged->setPosition(validated_move.to_c.x * WIDTH/8 + WIDTH/16 - OFFSET, validated_move.to_c.y * WIDTH/8 + WIDTH/16 - OFFSET);
    if (temp_index != -1) {
        std::forward_list<sf::Sprite>::iterator it = sprites.before_begin();
        std::advance(it, temp_index);
        sprites.erase_after(it);
    }
}



/*

int main() {
    
    /*
    google::dense_hash_map<Cords, Cords, std::hash<Cords>, cords_eq> a;
    a.set_empty_key(Cords{-1, -1});
    a[Cords{1, 1}] = Cords{1, 1};
    std::cout << a[Cords{1, 2}].x;
    */
    
    for (int i = 0; i < 8; i++) {
        incre8[i] = 1;
    }
    s_timer = 0;
    
    
    int frame_counter = 0;
    
    sf::Sprite* sprite_being_dragged;
    sprite_being_dragged = NULL;
    
    Move move, validated_move;
    bool trying_to_promote = false;

    // create the window
    sf::RenderWindow window(sf::VideoMode(WIDTH, WIDTH), "My window");
    window.setFramerateLimit(60);
    
    load_textures();
    set_promotional_sprites();
    
    
    sf::RectangleShape displaygrid[8][8];
    // create the Grid
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            // init rectangle
            sf::RectangleShape tempRect(sf::Vector2f(WIDTH/8, WIDTH/8));
            
            // Make checkerboard pattern
            if ((x + y) % 2 == 0) {
                tempRect.setFillColor(sf::Color(143, 101, 83));
            }
            else {
                tempRect.setFillColor(sf::Color(84, 54, 41));
            }
            
            // move to proper location and draw
            tempRect.setPosition(x * WIDTH/8, y * WIDTH/8);
            displaygrid[y][x] = tempRect;
        }
    }
    
    // Create the promotion rectangle
    sf::RectangleShape promotion_rectangle(sf::Vector2f(WIDTH/2, WIDTH/8));
    promotion_rectangle.setFillColor(sf::Color(26, 110, 8, 200));
    promotion_rectangle.setPosition(WIDTH / 4, WIDTH / 2 - WIDTH / 16);
    
    // init the chess board
    Board board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");

    std::vector<Move> moves;
    moves.reserve(256);
    board.generate_moves(moves);
    
    Search searcher(board);
    
    auto t1 = std::chrono::high_resolution_clock::now();
    /*
    int counter = 0;
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        board.print_move(*it, true);
        board.process_move(*it);
//        counter += board.Perft(5);
        std::cout << ": " << board.Perft(1) << std::endl;
//        board.debug_print();
//        board.debug_pins();
        board.undo_last_move();
    }
     */

    /*
//    std::cout << board.Perft(6) << std::endl;
//    std::cout<<counter;
    print_move(searcher.find_best_move(6), true);
    std::cout << '\n';

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    
    std::cout << "Time: " << ms_double.count() << "ms\n";
    std::cout << "s_timer: " << s_timer << "ms\n";
     */

    board.clear_attacks_on_king();
    board.set_texture_to_pieces();
    
    
    /*
    // Measuring FPS
    sf::Clock clock;
    int counter = 0;
    */
    
    
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (board.get_current_turn() == 0) {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left && !sprite_being_dragged)
                    {
                        if (trying_to_promote) {
                            int temp_index = locate_sprite_clicked_index(promotion_sprites_black, sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y, NULL);
                            if (temp_index != -1) {
                                switch (temp_index) {
                                    case 3:
                                        move.type = Promote_to_Queen;
                                        break;
                                    case 2:
                                        move.type = Promote_to_Rook;
                                        break;
                                    case 1:
                                        move.type = Promote_to_Bishop;
                                        break;
                                    case 0:
                                        move.type = Promote_to_Knight;
                                        break;
                                    default:
                                        std::cout << "Something went wrong about promotion. " << std::endl;
                                        break;
                                }

                                validated_move = board.request_move(move);
    //                            board.debug_print();
    
                                sf::Sprite* sprite = locate_sprite_clicked(sprites, validated_move.from_c.x * WIDTH/8 + WIDTH/16 - OFFSET, validated_move.from_c.y * WIDTH/8 + WIDTH/16 - OFFSET);
    
                                set_single_promotion_texture(!board.get_current_turn(), 3 - temp_index, *sprite);
                                normal_move_sprite_handler(validated_move, sprite);
    
                                trying_to_promote = false;
                            }
                        }
                        else {
                            // save the sprite being dragged
                            sprite_being_dragged = locate_sprite_clicked(sprites, sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
                            if (sprite_being_dragged) {
                                move.from_c = find_grid_bounds(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
                            }
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (sprite_being_dragged) {
    
                            move.to_c = find_grid_bounds(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
                            move.type = Normal;
                            trying_to_promote = board.is_trying_to_promote(move);
    
    
                            if (!trying_to_promote) {
    
                                int castle_side_value, en_passant_side_value;
                                if (board.get_current_turn() == 1) {
                                    en_passant_side_value = 4;
                                    castle_side_value = 0;
                                }
                                else {
                                    en_passant_side_value = 3;
                                    castle_side_value = 7;
                                }
    
    
                                validated_move = board.request_move(move);
    
                                // This section checks and handles the validity of move, including drawing the sprite
                                if (validated_move.type != Illegal) {
                                    std::cout << board.generate_FEN() << '\n';
    
                                    // If move is valid, set the sprite to the new position, delete the sprite that was residing in the to_location, and register the move with the board.
    
    
                                    // Standard move (Move piece from cords A to cords B, delete the sprite that is currently at the target location)
                                    normal_move_sprite_handler(validated_move, sprite_being_dragged);
    
                                    switch (validated_move.type) {
                                        case Castle_Kingside:
                                        case Castle_Queenside:
                                            // Castle handler
                                            castle_sprite_handler(castle_side_value, validated_move);
                                            break;
                                        case En_Passant:
                                            // En Passant Handler
                                            en_passant_sprite_handler(en_passant_side_value, validated_move);
                                            break;
                                        default:
                                            break;
                                    }

                                    frame_counter = 0;
    
    //                                board.debug_print();
                                }
                                else {
                                    // If move isn't valid, return sprite to original position and do nothing
                                    sprite_being_dragged->setPosition(move.from_c.x * WIDTH/8 + WIDTH/16 - OFFSET, move.from_c.y * WIDTH/8 + WIDTH/16 - OFFSET);
                                }
                            }
                            else {
                                // If move is trying to promote, return sprite to original position and do nothing
                                sprite_being_dragged->setPosition(move.from_c.x * WIDTH/8 + WIDTH/16 - OFFSET, move.from_c.y * WIDTH/8 + WIDTH/16 - OFFSET);
                            }
                            sprite_being_dragged = NULL;
                        }
                    }
                }
            }
            
            else if (frame_counter > 2) {
                // Now, do the AI's move
                
                Search search(board);
                Move best_move = search.find_best_move(6);
                
                
                sf::Sprite* AI_sprite;
                AI_sprite = locate_sprite_clicked(sprites, best_move.from_c.x * WIDTH/8 + WIDTH/16 - OFFSET, best_move.from_c.y * WIDTH/8 + WIDTH/16 - OFFSET);
                normal_move_sprite_handler(best_move, AI_sprite);
                
                int castle_side_value, en_passant_side_value;
                if (board.get_current_turn() == 1) {
                    en_passant_side_value = 4;
                    castle_side_value = 0;
                }
                else {
                    en_passant_side_value = 3;
                    castle_side_value = 7;
                }
                
                switch (best_move.type) {
                    case Castle_Kingside:
                    case Castle_Queenside:
                        // Castle handler
                        castle_sprite_handler(castle_side_value, best_move);
                        break;
                    case En_Passant:
                        // En Passant Handler
                        en_passant_sprite_handler(en_passant_side_value, validated_move);
                        break;
                    default:
                        break;
                }
                
                board.process_move(best_move);
            }
        }

        
        // Do Calcs/Pos changes
        
        if (sprite_being_dragged) {
            sprite_being_dragged->setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
        }
        
        
        
        
        // clear the window with black color
        window.clear(sf::Color::Black);

        // Draw stage
        
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                window.draw(displaygrid[y][x]);
            }
        }
        
        // Draw the pieces
        draw_pieces(&window);
        

        if (trying_to_promote) {
            window.draw(promotion_rectangle);
            draw_promotion_pieces(&window, !board.get_current_turn());
        }
        
        
        
        
        // end the current frame
        window.display();
        frame_counter++;
        
        /*
        // Measuring FPS
        counter++;
        if (counter == 5000) {
            std::cout << (5000.0 / clock.restart().asSeconds()) << std::endl;
            counter = 0;
        }
        */
    }

    return EXIT_SUCCESS;
}

*/



int main() {
    
    
    return 0;
}
