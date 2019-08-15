// This is the CPP file you will edit and turn in. (TODO: Remove this comment!)

#include "tilelist.h"
#include <algorithm>
#include "strlib.h"
using namespace std;

TileList::TileList() {
    // Constructor
    front = nullptr;
    back = nullptr;
}

TileList::~TileList() {
    // Destructor
    clear();
}

void TileList::addBack(int x, int y, int width, int height, string color) {
    // Add a new tile to the back of the TileList
    if (front == nullptr && back == nullptr) {
        // TileList is empty
        // By using "new", the pointer object will not be cleaned up when functions goes out of scope
        front = new TileNode(x, y, width, height, color);
        back = front;
    } else {
        // TileList is not empty
        TileNode* current = back;
        current->next = new TileNode(x, y, width, height, color);
        back = current->next;
        back->prev = current;
    }

}

void TileList::addFront(int x, int y, int width, int height, string color) {
    // Add a new tile to the front of the TileList
    if (front == nullptr && back == nullptr) {
        // TileList is empty
        front = new TileNode(x, y, width, height, color);
        back = front;
    } else {
        // TileList is not empty
        TileNode* current = front;
        current->prev = new TileNode(x, y, width, height, color);
        front = current->prev;
        front->next = current;
    }
}

void TileList::clear() {
    // Remove all nodes from the list
    while (front != nullptr) {
        TileNode* removed = detachTileFromList(front);
        delete removed;
    }
}

void TileList::debug() {
    // Left blank
}

void TileList::drawAll(GWindow& window) const {
    // Draw all the TileNodes in the list in the given window
    // Start from back of the list (lower in z-ordering will be drawn first)
    TileNode* current = back;
    // If the list is empty, there's nothing to do
    if (current != nullptr) {
        current->draw(window);
        while (current->prev != nullptr) {
            current = current->prev;
            current->draw(window);
        }
    }
}

TileNode* TileList::getBack() const {
    // Get the TileNode pointer at the back of the list
    return back;
}

TileNode* TileList::getFront() const {
    // Get the TileNode pointer at the back of the list
    return front;
}

bool TileList::highlight(int x, int y) {
    // Highlight the topmost tile at point (x, y) by changing its color to "yellow"
    TileNode* current = findTileByCoordinateFromFront(x, y);
    if (current != nullptr) {
        current->color = "#FFFF00";
        return true;
    } else {
        return false;
    }
}

bool TileList::lower(int x, int y) {
    // Move the topmost tile at point (x, y) to the very back of the list
    TileNode* current = findTileByCoordinateFromFront(x, y);
    if (current != nullptr) {
        // Detach the current node from the list
        TileNode* lowered = detachTileFromList(current);
        // Move the detached node to the back of the list
        current = back;
        current->next = lowered;
        back = current->next;
        back->prev = current;
        return true;
    } else {
        return false;
    }
}

void TileList::merge(int x, int y) {
    // Combine all the tiles at point (x, y) into a single tile
    // whose size is EXACTLY large enough to cover all those tiles
    TileNode* current = findTileByCoordinateFromFront(x, y);
    string mergeColor = ""; int x_min = 0; int y_min = 0; int x_max = 0; int y_max = 0;

    if (current != nullptr) {
        mergeColor += current->color;
        x_min = current->x;
        y_min = current->y;
        x_max = current->x + current->width;
        y_max = current->y + current->height;
    }

    // Find the boundary of the merged tile
    while (current != nullptr) {
        TileNode* removed = detachTileFromList(current);
        delete removed;
        current = findTileByCoordinateFromFront(x, y);
        if (current == nullptr) {
            break;
        } else {
            if (current->x < x_min) {
                x_min = current->x;
            }
            if (current->y < y_min) {
                y_min = current->y;
            }
            if (current->x + current->width > x_max) {
                x_max = current->x + current->width;
            }
            if (current->y + current->height > y_max) {
                y_max = current->y + current->height;
            }
        }
    }

    // Create the merged tile and move it to the front of the list
    if (mergeColor != "") {
        int width_max = x_max - x_min;
        int height_max = y_max - y_min;
        TileNode* merged = new TileNode(x_min, y_min, width_max, height_max, mergeColor);
        current = front;
        current->prev = merged;
        front = current->prev;
        front->next = current;
    }
}

bool TileList::raise(int x, int y) {
    // Move the topmost tile at point (x, y) to the very front of the list
    TileNode* current = findTileByCoordinateFromFront(x, y);
    if (current != nullptr) {
        // Detach the current node from the list
        TileNode* raised = detachTileFromList(current);
        // Move the detached node to the front of the list
        current = front;
        current->prev = raised;
        front = current->prev;
        front->next = current;
        return true;
    } else {
        return false;
    }
}

bool TileList::remove(int x, int y) {
    // Remove the selected tile from the list
    TileNode* current = findTileByCoordinateFromFront(x, y);
    if (current != nullptr) {
        TileNode* removed = detachTileFromList(current);
        delete removed;
        return true;
    } else {
        return false;
    }
}

int TileList::removeAll(int x, int y) {
    // Remove ALL tiles from the list at point (x, y) and return the number of removed nodes
    TileNode* current = findTileByCoordinateFromFront(x, y);
    int count = 0;
    while (current != nullptr) {
        count++;
        TileNode* removed = detachTileFromList(current);
        delete removed;
        current = findTileByCoordinateFromFront(x, y);
    }
    return count;
}

//************************************Below are helper functions****************************
TileNode* TileList::findTileByCoordinateFromFront(int x, int y) {
    TileNode* current = front;
    if (front == nullptr && back == nullptr) {
        return nullptr;
    } else {
        if (current->contains(x, y)) {
            return current;
        }
        while (current->next != nullptr) {
            current = current->next;
            if (current->contains(x, y)) {
                return current;
            }
        }
        return nullptr;
    }
}

TileNode* TileList::detachTileFromList(TileNode *node) {
    if (node->prev == nullptr && node->next == nullptr) {
        // Case 1: Only one node in the list
        front = nullptr;
        back = front;
    } else if (node->prev == nullptr) {
        // Case 2: Current pointer is at the front of the list
        front = front->next;
        front->prev = nullptr;
    } else if (node->next == nullptr) {
        // Case 3: Current pointer is at the back of the list
        back = back->prev;
        back->next = nullptr;
    } else {
        // Case 4: Current pointer is in the middle of the list
        node->prev->next = node->next;
        node->next->prev = node->prev;
        node->next = nullptr;
        node->prev = nullptr;
    }
    return node;
}

