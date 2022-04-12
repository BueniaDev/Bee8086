#ifndef BEE8086_MDA
#define BEE8086_MDA

#include <iostream>
#include <cstdint>
using namespace std;

namespace beemda
{
    class BeeMDA
    {
	public:
	    BeeMDA()
	    {

	    }

	    ~BeeMDA()
	    {

	    }

	    void writeReg(uint8_t data)
	    {
		crtc_reg = (data & 0x1F);
	    }

	    void writeData(uint8_t data)
	    {
		cout << "Writing value of " << hex << int(data) << " to MDA register of " << dec << int(crtc_reg) << endl; 
	    }

	    void writeControl(uint8_t data)
	    {
		if (!testbit(data, 0))
		{
		    cout << "Undefined behavior detected" << endl;
		    return;
		}

		is_blink_enabled = testbit(data, 5);
		is_video_enabled = testbit(data, 3);

		cout << "Blinking: " << ((is_blink_enabled) ? "Enabled" : "Disabled") << endl;
		cout << "Video output: " << ((is_video_enabled) ? "Enabled" : "Disabled") << endl;
		cout << endl;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, int bit)
	    {
		return ((reg >> bit) & 1) ? true : false;
	    }

	    bool is_blink_enabled = false;
	    bool is_video_enabled = false;

	    int crtc_reg = 0;
    };
}


#endif // BEE8086_MDA