#ifndef BEEFLOPPY_H
#define BEEFLOPPY_H

#include <iostream>
#include <vector>
#include <cstdint>
using namespace std;

struct BeeFloppy
{
    vector<uint8_t> filedata;
    uint32_t fileoffs = 0;

    int numcylinders = 0;
    int numheads = 0;
    int numsectors = 0;

    void open(vector<uint8_t> data)
    {
	filedata = vector<uint8_t>(data.begin(), data.end());

	numcylinders = 80;
	numsectors = 18;
	numheads = 2;

	if (filedata.size() <= 1228800)
	{
	    numsectors = 15;
	}

	if (filedata.size() <= 737280)
	{
	    numsectors = 9;
	}

	if (filedata.size() <= 368640)
	{
	    numcylinders = 40;
	    numsectors = 9;
	}

	if (filedata.size() <= 163840)
	{
	    numcylinders = 40;
	    numsectors = 8;
	    numheads = 1;
	}
    }

    void close()
    {
	filedata.clear();
	numcylinders = 0;
	numsectors = 0;
	numheads = 0;
    }

    bool seek(int cylinder_num, int head_num, int sector_num)
    {
	uint32_t lba = toLBA(cylinder_num, head_num, sector_num);
	cout << "LBA is " << dec << lba << endl;

	uint32_t offset = (lba * 512);

	if (offset >= filedata.size())
	{
	    return false;
	}

	fileoffs = offset;
	return true;
    }

    vector<uint8_t> readSector()
    {
	vector<uint8_t> result;
	if (fileoffs >= filedata.size())
	{
	    return result;
	}

	if ((fileoffs + 512) > filedata.size())
	{
	    size_t actualSize = (filedata.size() - fileoffs);
	    result.resize(actualSize, 0);
	}
	else
	{
	    result.resize(512, 0);
	}

	for (size_t index = 0; index < result.size(); index++)
	{
	    result[index] = filedata.at(fileoffs++);
	}

	return result;
    }

    uint32_t toLBA(int cylinder, int head, int sector)
    {
	return (cylinder * numheads + head) * numsectors + (sector - 1);
    }

    void fromLBA(int lba, int &cylinder, int &head, int &sector)
    {
	cylinder = lba / (numheads * numsectors);
	head = (lba / numsectors) % numheads;
	sector = (lba % numsectors) + 1;
    }
};

#endif // BEEFLOPPY_H