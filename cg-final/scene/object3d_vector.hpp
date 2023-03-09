#pragma once

#include "assert.h"
class Triangle;

// ====================================================================
// ====================================================================

// Store an collection of Object3D.  The total number to be stored is
// not known initially.  Resize as necessary.

// ====================================================================
// ====================================================================

class Object3DVector {

public:

    // CONSTRUCTOR
    Object3DVector() {
        num_objects = 0;
        size = 10;
        objects = new Triangle * [size];
        for (int i = 0; i < size; i++)
            objects[i] = NULL;
    }

    // DESTRUCTOR
    ~Object3DVector() {
        // don't delete the objects, just the array to store the pointers
        delete[] objects;
    }

    // ACCESSORS
    int getNumObjects() { return num_objects; }
    Triangle* getObject(int i) {
        assert(i >= 0 && i < num_objects);
        assert(objects[i] != NULL);
        return objects[i];
    }

    // MODIFIERS
    void addObject(Triangle* o) {
        assert(o != NULL);
        if (size == num_objects) {
            // double the size of the array and copy the pointers
            int new_size = size * 2;
            Triangle** new_objects = new Triangle * [new_size];
            int i;
            for (i = 0; i < size; i++) {
                new_objects[i] = objects[i];
            }
            for (i = size; i < 2 * size; i++) {
                new_objects[i] = NULL;
            }
            delete[] objects;
            objects = new_objects;
            size = new_size;
        }
        objects[num_objects] = o;
        num_objects++;
    }

private:

    // REPRESENTATION
    Triangle** objects;
    int size;
    int num_objects;

};

// ====================================================================
// ====================================================================
