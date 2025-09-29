#include<fstream>
#include<iostream>
#include<valarray>
#include<array>
#include<string>
#include<sstream>

#include "Molecule.hpp"
#include "Element.hpp"


namespace chem{
    class Xyz : public MoleculeFile{
        public:
            Xyz(const std::string& filename);
            void autoCentering(void);
        private:
            void loadFile(const std::string& filename);
    };
}

chem::Xyz::Xyz(const std::string& filename)
{
    this->loadFile(filename);
}

void chem::Xyz::loadFile(const std::string& filename){
    std::ifstream file(filename);
    if (file.fail())
    {
        std::cout << filename << " file not found" << std::endl;
        return;
    }
    std::cout << filename << " file opened successfully" << std::endl;

    std::string buff;
    std::getline(file, buff);  // Atom count
    unsigned short atom_count = std::stoi(buff);
    this->atomNumberArray.resize(atom_count);
    this->atomCoordArray.resize(atom_count);

    std::getline(file, buff);  // Title

    std::string atom_type;
    std::array<double, 3> atom_coords = {0.0, 0.0, 0.0};
    for (int i = 0; i < atom_count; i++)
    {
        std::getline(file, buff);
        std::stringstream buff_ss(buff);
        buff_ss >> atom_type >> atom_coords[0] >> atom_coords[1] >> atom_coords[2];
        this->atomNumberArray[i] = chem::getId(atom_type) + 1;
        this->atomCoordArray[i] = atom_coords;
    }
    if (file.is_open()){file.close();}
}

void chem::Xyz::autoCentering(void) {
    std::array<double, 3> geom_center = this->getGeomCenter();
    for (int i = 0; i < this->atomCoordArray.size(); i++) {
        this->atomCoordArray[i][0] -= geom_center[0];
        this->atomCoordArray[i][1] -= geom_center[1];
        this->atomCoordArray[i][2] -= geom_center[2];
    }
}
