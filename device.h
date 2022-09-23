#pragma once

namespace NESEmulator {
    
template<class T>
class Device {
public:
    friend T;
    static T& the()
    {
	static T sDevice;
	return sDevice;
    }
protected:
    Device() { }
};

}
