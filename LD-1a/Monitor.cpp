//
// Created by justas327 on 2019-10-10.
//

#include "Monitor.h"

Monitor::Monitor(int size) {
    list = new Car[size];
    this->size = 0;
    this->capacity = size;
    this->finished = false;
}

Monitor::~Monitor() {
    delete[] list;
}

void Monitor::add(Car &new_car, bool sorted) {
    {
        unique_lock<mutex> lock(m);
        while (this->size == capacity) {
            cv.wait(lock);
        }

        if (this->size != this->capacity) {
            if (!sorted) {
                list[size++] = new_car;
            } else {
                int index = find_index(new_car);
                shift_list(index);
                list[index] = new_car;
                size++;
            }
        }
    }

    cv.notify_all();
}

unsigned int Monitor::get_size() {
    return this->size;
}

int Monitor::find_index(Car &car) {
    int condition = car.getNumber();
    int index = 0;

    for (unsigned int i = 0; i < size; i++) {
        if (condition < list[i].getNumber())
            return i;

        index++;
    }

    return index;
}

void Monitor::shift_list(int index) {
    for (int i = size; i > index; i--) {
        list[i] = list[i - 1];
    }
}

Car Monitor::get(unsigned int index) {
    Car *found_car;
    m.lock();
    if (size == 0) {
        found_car = nullptr;
    }

    found_car = &list[index];

    m.unlock();
    return (*found_car);
}

void Monitor::data_loading_finished() {
    finished = true;
}

bool Monitor::is_loading_finished() {
    return this->finished;
}

Car Monitor::pop() {
    Car result;
    {
        unique_lock<mutex> lock(m);
        while (size == 0 && !finished) {
            cv.wait(lock);
        }

        if (size != 0) {
            result = list[size - 1];
            size--;
        }
    }

    cv.notify_all();
    return result;
}