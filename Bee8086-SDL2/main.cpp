#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <cstdint>
#include <SDL2/SDL.h>
#include <Bee8086/bee8086.h>
#include "beefloppy.h"
using namespace bee8086;
using namespace std;

class SDL2Frontend : public Bee8086Interface
{
    public:
	SDL2Frontend()
	{

	}

	~SDL2Frontend()
	{

	}

	void printusage()
	{
	    cout << "Usage: example [floppy image]" << endl;
	}

	bool init()
	{
	    if (!load_bios())
	    {
		cout << "Unable to initialize frontend." << endl;
		return false;
	    }

	    if (!load_floppy())
	    {
		cout << "Unable to initialize frontend." << endl;
		return false;
	    }

	    memory.resize(0x100000, 0);

	    core.setinterface(this);
	    core.init(0xF000, 0x100);

	    if (SDL_Init(SDL_INIT_VIDEO) < 0)
	    {
		return sdlerror("SDL could not be initialized!");
	    }

	    window = SDL_CreateWindow("Bee8086-SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 200, 200, SDL_WINDOW_SHOWN);

	    if (window == NULL)
	    {
		return sdlerror("Window could not be created!");
	    }

	    surface = SDL_GetWindowSurface(window);

	    return true;
	}

	bool sdlerror(string message)
	{
	    cout << message << " SDL_Error: " << SDL_GetError() << endl;
	    return false;
	}

	void shutdown()
	{
	    memory.clear();
	    bios.clear();
	    disk_a.close();
	    core.shutdown();
	    SDL_DestroyWindow(window);
	    SDL_Quit();
	}

	bool run()
	{
	    SDL_Event ev;

	    while (SDL_PollEvent(&ev))
	    {
		switch (ev.type)
		{
		    case SDL_QUIT: return false; break;
		}
	    }

	    runcore();
	    return true;
	}

	bool getargs(int argc, char *argv[])
	{
	    if (argc < 2)
	    {
		printusage();
		return false;
	    }

	    floppy_name = argv[1];

	    return true;
	}

	bool load_bios()
	{
	    vector<uint8_t> biostemp = loadfile(bios_name);

	    if (biostemp.empty())
	    {
		return false;
	    }

	    if (biostemp.size() > 0x8000)
	    {
		cout << "Error: BIOS size mismatch" << endl;
		return false;
	    }

	    bios.resize(0x8000, 0);

	    for (size_t index = 0; index < biostemp.size(); index++)
	    {
		bios[(0x100 + index)] = biostemp[index];
	    }

	    biostemp.clear();
	    return true;
	}

	bool load_floppy()
	{
	    vector<uint8_t> floppy_temp = loadfile(floppy_name);

	    if (floppy_temp.empty())
	    {
		return false;
	    }

	    disk_a.open(floppy_temp);
	    return true;
	}

	void runcore()
	{
	    core.debugoutput();
	    core.runinstruction();
	}

	uint8_t readByte(uint32_t addr)
	{
	    uint8_t temp = 0;

	    if (addr < 0xF0000)
	    {
		temp = memory[addr];
	    }
	    else if (addr < 0xF8000)
	    {
		temp = bios[(addr & 0xFFFF)];
	    }
	    else if (addr < 0xFFFF0)
	    {
		temp = memory[addr];
	    }
	    else
	    {
		temp = biosdata[(addr & 0xF)];
	    }

	    return temp;
	}

	void writeByte(uint32_t addr, uint8_t val)
	{
	    if (addr < 0xF0000)
	    {
		memory[addr] = val;
	    }
	    else if (addr < 0xF8000)
	    {
		return;
	    }
	    else if (addr < 0xFFFF0)
	    {
		memory[addr] = val;
	    }
	    else
	    {
		biosdata[(addr & 0xF)] = val;
	    }
	}

	uint8_t portIn(uint16_t port)
	{
	    uint16_t temp = 0;
	    cout << "Reading from port of " << hex << (int)(port) << endl;
	    exit(0);
	    return temp;
	}

	void portOut(uint16_t port, uint8_t val)
	{
	    switch (port)
	    {
		case 0x4F8:
		{
		    is_unimp_int = true;
		    int_num = val;
		}
		break;
		case 0x4F9:
		{
		    if (is_unimp_int == true)
		    {
			cout << "Unimplemented interrupt of " << hex << int(int_num) << ", service number of " << hex << int(val) << endl;
			exit(1);
		    }
		}
		break;
		default:
		{
		    cout << "Writing to port of " << hex << (int)(port) << ", value of " << hex << (int)(val) << endl;
		    exit(0);
		}
		break;
	    }
	}

	bool isInterruptOverride(uint8_t int_num)
	{
	    return (int_num == 0x13);
	}

	void interruptOverride(Bee8086 &state, uint8_t int_num)
	{
	    uint8_t service_num = state.get_ah();

	    switch (int_num)
	    {
		case 0x13:
		{
		    switch (service_num)
		    {
			case 2:
			{
			    size_t num_sectors = state.get_al();
			    uint16_t cylinder_temp = state.get_cx();
			    size_t cylinder_num = ((cylinder_temp >> 8) | ((cylinder_temp & 0xC0) << 2));
			    size_t sector_num = (cylinder_temp & 0x3F);

			    size_t head_num = state.get_dh();
			    size_t drive_num = state.get_dl();

			    if (drive_num != 0)
			    {
				cout << "Invalid drive number of " << hex << int(drive_num) << endl;
				exit(1);
			    }

			    if (!disk_a.seek(cylinder_num, head_num, sector_num))
			    {
				cout << "Invalid seek" << endl;
				exit(1);
			    }

			    if (num_sectors == 0)
			    {
				cout << "Invalid sector number" << endl;
				exit(1);
			    }

			    uint32_t sector_addr = convertSeg(state.get_es(), state.get_bx());

			    size_t num_sectors_read = 0;

			    for (size_t i = 0; i < num_sectors; i++)
			    {
				vector<uint8_t> sector_vec = disk_a.readSector();

				if (sector_vec.empty())
				{
				    break;
				}

				for (size_t j = 0; j < sector_vec.size(); j++)
				{
				    size_t sector_offs = ((num_sectors_read * 512) + j);
				    writeByte((sector_addr + sector_offs), sector_vec.at(j));
				}

				num_sectors_read += 1;
			    }

			    state.set_ah(0);
			    state.set_al(num_sectors_read);
			}
			break;
			default:
			{
			    cout << "Unrecognized int 13h service number of " << hex << int(service_num) << endl;
			    exit(1);
			}
			break;
		    }
		}
		break;
		default:
		{
		    cout << "Unrecognized interrupt of " << hex << int(int_num) << endl;
		    exit(1);
		}
		break;
	    }
	}

	vector<uint8_t> loadfile(string filename)
	{
	    vector<uint8_t> temp;
	    ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);
	    
	    if (!file.is_open())
	    {
		cout << "Error: could not load " << filename << endl;
		return temp;
	    }

	    streampos size = file.tellg();
	    temp.resize(size, 0);
	    file.seekg(0, ios::beg);
	    file.read((char*)temp.data(), size);
	    cout << filename << " succesfully loaded." << endl;
	    file.close();
	    return temp;
	}

	uint32_t convertSeg(uint16_t seg, uint16_t offs)
	{
	    return (((seg << 4) + offs) & 0xFFFFF);
	}

    private:
	vector<uint8_t> memory;
	vector<uint8_t> bios;
	array<uint8_t, 0x10> biosdata;

	string bios_name = "kujobios.bin";
	string floppy_name = "";

	Bee8086 core;

	BeeFloppy disk_a;

	SDL_Window *window = NULL;
	SDL_Surface *surface = NULL;

	bool is_unimp_int = false;
	uint8_t int_num = 0;
	uint8_t service_num = 0;
};

int main(int argc, char *argv[])
{
    SDL2Frontend core;

    if (!core.getargs(argc, argv))
    {
	return 1;
    }

    if (!core.init())
    {
	cout << "Could not initialize frontend." << endl;
	return 1;
    }

    while (core.run());
    core.shutdown();
    return 0; 
}