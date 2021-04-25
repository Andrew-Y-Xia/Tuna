//
//  Data_structs.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Data_structs.hpp"

bool Cords::operator==(const Cords c2) {
    return (this->x == c2.x && this->y == c2.y);
}
bool Cords::operator!=(const Cords c2) {
    return !(*this == c2);
}
Cords::Cords() {
    x = -1;
    y = -1;
}
Cords::Cords(int a, int b) {
    x = a; y = b;
}
