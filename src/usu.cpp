#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

struct Usuario {
    char usuario[50];
    char contrasena[50];
};

void crearUsuariosDePrueba() {
    ofstream archivo("usuarios.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo de usuarios." << endl;
        return;
    }

    Usuario usuarios[] = {
        {"johan@gmail.com", "password1"},
        {"gaby@gmail.com", "password2"}
    };

    for (const auto& usuario : usuarios) {
        archivo.write(reinterpret_cast<const char*>(&usuario), sizeof(Usuario));
    }

   
    archivo.close();
}


void eliminarUsuario() {
    string correo;
    cout << "Ingrese el correo electrónico del usuario que desea eliminar: ";
    cin >> correo;

    // Verificar tamaño del archivo
    ifstream archivoLectura("usuarios.bin", ios::binary | ios::ate);
    if (!archivoLectura.is_open()) {
        cerr << "Error al abrir el archivo de usuarios." << endl;
        return;
    }
    streamsize size = archivoLectura.tellg();
    archivoLectura.seekg(0, ios::beg);
    cout << "Tamaño del archivo: " << size << " bytes" << endl;

    // Abrir un archivo temporal en modo binario de escritura
    ofstream archivoTemporal("usuarios_temp.bin", ios::binary);
    if (!archivoTemporal.is_open()) {
        cerr << "Error al abrir el archivo temporal." << endl;
        archivoLectura.close();
        return;
    }

    Usuario usuario;
    bool encontrado = false;
    size_t usuariosLeidos = 0;

    // Leer todos los usuarios del archivo original
    while (archivoLectura.read(reinterpret_cast<char*>(&usuario), sizeof(Usuario))) {
        usuariosLeidos++;
        cout << "Leyendo usuario: " << usuario.usuario << " contraseña: " << usuario.contrasena << endl;
        string usuarioStr(usuario.usuario);
        if (usuarioStr == correo) {
            encontrado = true;
            cout << "Usuario encontrado: " << usuarioStr << endl;
            // Usuario encontrado, no lo escribimos en el archivo temporal
        } else {
            cout << "Escribiendo usuario: " << usuarioStr << " en el archivo temporal" << endl;
            archivoTemporal.write(reinterpret_cast<const char*>(&usuario), sizeof(Usuario));
        }
    }

    archivoLectura.close();
    archivoTemporal.close();

    cout << "Usuarios leídos: " << usuariosLeidos << endl;

    if (!encontrado) {
        cout << "El usuario especificado no existe en el archivo." << endl;
        remove("usuarios_temp.bin");
        return;
    }

    if (remove("usuarios.bin") != 0) {
        cerr << "Error al eliminar el archivo original." << endl;
        return;
    }

    if (rename("usuarios_temp.bin", "usuarios.bin") != 0) {
        cerr << "Error al renombrar el archivo temporal." << endl;
        return;
    }

    cout << "Usuario eliminado correctamente del archivo." << endl;
}



int main() {
   
    eliminarUsuario();
    return 0;
}
