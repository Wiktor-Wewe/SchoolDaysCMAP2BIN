#include <iostream>
#include <fstream>
#include <vector>

struct element 
{
    int id;
    int x;
    int y;
    int w;
    int h;
};

element* getElementWithIdOrMakeNew(int id, int position, int x, std::vector<element*>& list) 
{
    for (int i = 0; i < list.size(); i++) {
        if (list[i]->id == id) {
            return list[i];
        }
    }

    element* newElement = new element;
    newElement->id = id;
    newElement->x = (position - 1) % x;
    newElement->y = ((position - 1) / x) - 75; // -75 because CMAP is for 800x600 but real PNG file is 800x450 -> (600-450)/2 = 75 :P
    newElement->w = 0;
    newElement->h = 0;
    list.push_back(newElement);
    return newElement;
}

void updateInfo(int position, int x, int y, element* e)
{
    if ((x - e->x - (x - ((position - 1) % x))) > e->w) {
        e->w = (x - e->x - (x - ((position - 1) % x)));
    }
    if ( y - e->y - (y - ((position - 1) / x) ) > e->h) {
        e->h = y - e->y - (y  - ((position - 1) / x));
    }
}

void makeBinFile(std::string name, std::vector<element*>& list, int x, int y) 
{
    std::fstream fileOUT;
    fileOUT.open(name + ".bin", std::ios::out | std::ios::binary);

    if (!fileOUT.good()) {
        printf("unable to make bin file!\n");
        return;
    }


    uint32_t buff32; uint16_t buff16;
    fileOUT << "CMAPbin ";

    buff32 = x;
    fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32));

    buff32 = y;
    fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32));

    buff32 = list.size();
    fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32));

    for (int i = 0; i < list.size(); i++) {
        buff16 = list[i]->id;
        fileOUT.write(reinterpret_cast<const char*>(&buff16), sizeof(buff16));
        buff32 = list[i]->x;
        fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32));
        buff32 = list[i]->y;
        fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32));
        buff32 = list[i]->w;
        fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32));
        buff32 = list[i]->h;
        fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32));
    }

    fileOUT.close();
    printf("bin file was created\n");
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("usage: SchoolDaysCMAP2BIN [YOUR CMAP FILE].CMAP -> show info\n");
        printf("usage: SchoolDaysCMAP2BIN [YOUR CMAP FILE].CMAP -b -> make binnary file\n\n");
        return 1;
    }

    std::fstream fileIN;
    fileIN.open(argv[1], std::ios::in | std::ios::binary);

    uint32_t x, y;
    fileIN.read(reinterpret_cast<char*>(&x), sizeof(int));
    fileIN.read(reinterpret_cast<char*>(&y), sizeof(int));

    printf("x: %i\ny: %i\n", x, y);

    std::vector<element*> elements;

    uint8_t buff;
    while (!fileIN.eof()) {
        fileIN.read(reinterpret_cast<char*>(&buff), sizeof(buff));
        if (buff != 0x00) {
            updateInfo((int)fileIN.tellg() - 8, x, y, getElementWithIdOrMakeNew(buff, (int)fileIN.tellg() - 8, x, elements));
        }
    }

    for (int i = 0; i < elements.size(); i++) {
        printf("element id: %i\nx: %i\ny: %i\nw: %i\nh: %i\n\n", elements[i]->id, elements[i]->x, elements[i]->y, elements[i]->w, elements[i]->h);
        //printf("element id: 0x%X\nx: 0x%X\ny: 0x%X\nw: 0x%X\nh: 0x%X\n\n\n", elements[i]->id, elements[i]->x, elements[i]->y, elements[i]->w, elements[i]->h);
    }

    if (argc > 2) {
        std::string parameter(argv[2]);
        if (parameter == "-b") {
            makeBinFile(argv[1], elements, x, y);
        }
    }

    fileIN.close();
    return 0;
}
