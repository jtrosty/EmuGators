#pragma once

template<class T>
class Badge {
    friend T;
    Badge() { }
};
