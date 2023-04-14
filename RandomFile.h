#include <iostream>
#include <fstream>
#include <cstdio>
#include <map>
#include <string>

using namespace std;

struct Record
{
    char nombre[30];
    char carrera[20];
    int ciclo;

    void setData() {
        cout << "Alumno:";
        cin >> nombre;
        cout << "Carrera: ";
        cin >> carrera;
        cout << "Ciclo: ";
        cin >> ciclo;
    }

    string getKey() {
        return string(nombre) + "|" + string(carrera) + "|" + to_string(ciclo);
    }

    void showData() {
        cout << "\nNombre: " << nombre;
        cout << "\nCarrera: " << carrera;
        cout << "\nCiclo : " << ciclo;
    }    
};

class RandomFile {
private:
    string fileName;
    string indexName;
    //map: mantiene ordenado las entradas
    map<string, long> index;

public:
    RandomFile(string _fileName) {
        this->fileName = _fileName;
        this->indexName = _fileName + "_ind";
        readIndex();
    }
    
    ~RandomFile(){
        writeIndex();
    }

    /*
    * leer el indice desde disco
    */
    void readIndex()
    {
      ifstream file;
      file.open(this->fileName, ios::in|ios::binary);
      file.seekg(0, ios::end);
      int size_file = file.tellg();
      file.seekg(0, ios::beg);
      int size_current = 0;
      if (!file.is_open()) {
        cout << "Error al abrir el archivo " << this->fileName << endl;
      } else {
        while (size_current != size_file) {
            string key = 0;
            long address = 0;
            file.read((char*)&key, sizeof(string));
            file.read((char*)&address, sizeof(long));
            index[key] = address;
            size_current = 0;
        }
      }
      file.close();
    }

    /*
    * Regresa el indice al disco
    */
    void writeIndex(){
        ofstream file(this->indexName, ios::out|ios::binary);
        for (auto& entry: index) {
            file.write(entry.first.c_str(), 30);
            file.write((char*)&entry.second, sizeof(long));
        }
        file.close();
    }

    /*
    * Escribe el registro al final del archivo de datos. Se actualiza el indice. 
    */
    void write_record(Record record) {
       ofstream dataFile;
       dataFile.open(this->fileName, ios::app | ios::binary);
       long posFisica = dataFile.tellp();
       dataFile.write((char*)&record, sizeof(Record));
       this->index[record.getKey()] = posFisica;
       dataFile.close();
    }


    /*
    * Busca un registro que coincida con la key
    */
    Record* search(string key) {
        Record* result = nullptr;
        this->indexName = indexName;
        ifstream file;
        file.open(this->fileName, ios::binary);
        if (!file.is_open()) {
            cout << "Error al abrir el archivo " << this->fileName << endl;
        } else {
            file.seekg(index[indexName], ios::beg);
            int size = 0;
            file.read((char*)&size, sizeof(int));
            file.read((char*)result, size);
        }
        file.close();
        return result;
    }

    /*
   * Muestra todos los registros de acuerdo como fueron insertados en el archivo de datos
   */
    void scanAll() {
       ifstream file(this->fileName, ios::binary);
       Record record;
       while (file.read((char*)&record, sizeof(record))) {
        cout << "Nombre: " << record.nombre 
             << "| Carrera: " << record.carrera
             << "| Ciclo: " << record.ciclo
             << endl;
       } 
       file.close();
    }

    /*
   * Muestra todos los registros de acuerdo a como estan ordenados en el indice
   */
    void scanAllByIndex() {
       ifstream file(this->fileName, ios::binary);
       for (auto& entry: index) {
        file.seekg(entry.second, ios::beg);
        Record record;
        file.read((char*)&record, sizeof(record));
        cout << "Nombre: " << record.nombre 
             << "| Carrera: " << record.carrera
             << "| Ciclo: " << record.ciclo
             << endl;
       }
       file.close();
    }

};


