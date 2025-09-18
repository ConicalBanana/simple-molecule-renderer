#pragma once

#include<array>
#include<cmath>
#include<vector>


#include"Element.hpp"


namespace chem{
    double getBondLength(const std::array<double, 3>& atom_coord_1, const std::array<double, 3>& atom_coord_2);

    class MoleculeFile{
        public:
            MoleculeFile();
            ~MoleculeFile();
            
            std::vector<unsigned int> atomNumberArray;
            std::vector<std::array<double, 3>> atomCoordArray;

            const size_t size(void);
            const std::vector<std::array<unsigned short, 2>> getBondIndexArray(void);
            const std::vector<std::array<double, 6>> getBondVectorArray(void);
            const std::array<double, 3> getGeomCenter(void);
    };
}

double chem::getBondLength(const std::array<double, 3>& atom_coord_1, const std::array<double, 3>& atom_coord_2){
    return sqrt(
        pow(atom_coord_1[0] - atom_coord_2[0], 2)
        + pow(atom_coord_1[1] - atom_coord_2[1], 2)
        + pow(atom_coord_1[2] - atom_coord_2[2], 2)
    );
}

chem::MoleculeFile::MoleculeFile(){}

chem::MoleculeFile::~MoleculeFile(){}

const size_t chem::MoleculeFile::size(void){
    return (size_t)this->atomNumberArray.size();
}

const std::vector<std::array<unsigned short, 2>> chem::MoleculeFile::getBondIndexArray(void){
    double exp_bond_length = 0.;
    double actl_bond_length = 0.;
    std::vector<std::array<unsigned short, 2>> bond_index_array;

    for (int i = 0; i < this->atomNumberArray.size(); i++){
        for (int j = i + 1; j < this->atomNumberArray.size(); j++){
            exp_bond_length = chem::getExpectedBondLengh(this->atomNumberArray[i], this->atomNumberArray[j]);
            actl_bond_length = chem::getBondLength(this->atomCoordArray[i], this->atomCoordArray[j]);
            if (exp_bond_length > actl_bond_length){
                bond_index_array.push_back(
                    {static_cast<unsigned short>(i), static_cast<unsigned short>(j)}
                );
            }
        }
    }
    return bond_index_array;
}

const std::vector<std::array<double, 6>> chem::MoleculeFile::getBondVectorArray(void){
    std::vector<std::array<double, 6>> bond_vector_array;
    std::vector<std::array<unsigned short, 2>> bond_index_array =
        this->getBondIndexArray();
    std::array<double, 3> v1 = {0.0, 0.0, 0.0};
    std::array<double, 3> v2 = {0.0, 0.0, 0.0};
    for (int i = 0; i < bond_index_array.size(); i++){
        v1 = this->atomCoordArray[bond_index_array[i][0]];
        v2 = this->atomCoordArray[bond_index_array[i][1]];
        bond_vector_array.push_back(
            {
                v1[0], v1[1], v1[2],
                v2[0], v2[1], v2[2]
            }
        );
    }
    return bond_vector_array;
}

const std::array<double, 3> chem::MoleculeFile::getGeomCenter(void){
    std::array<double, 3> geom_center = {0.0, 0.0, 0.0};
    for (int i = 0; i < this->atomCoordArray.size(); i++){
        geom_center[0] += this->atomCoordArray[i][0];
        geom_center[1] += this->atomCoordArray[i][1];
        geom_center[2] += this->atomCoordArray[i][2];
    }
    geom_center[0] /= this->atomCoordArray.size();
    geom_center[1] /= this->atomCoordArray.size();
    geom_center[2] /= this->atomCoordArray.size();
    return geom_center;
}