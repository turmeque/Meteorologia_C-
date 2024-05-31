#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <regex> // Necesario para usar expresiones regulares

using namespace std;

const int MAX_DATOS = 2000; // Máximo número de datos meteorológicos
const string nombreArchivo = "datos_meteorologicos.txt";
//-------------------------------------------------------------------------------------------------------------//
struct DatosMeteorologicos
{
  int anio;
  int mes;
  int dia;
  float temperatura;
  int humedad;
  int velocidadViento;
  int presionAtmosferica;
  string condicionMeteorologica;
};

// Como lo dice su nombre, lee los datos metereológicos del .txt
int leerDatosMeteorologicos(string nombreArchivo, DatosMeteorologicos datos[],
                            int &numDatos, int &anioMin, int &mesMin,
                            int &diaMin, int &anioMax, int &mesMax,
                            int &diaMax)
{

  ifstream archivo(nombreArchivo);

  if (!archivo)
  {
    cerr << "Error al abrir el archivo: " << nombreArchivo << endl;
    return 1;
  }

  string encabezado;
  getline(archivo, encabezado);

  anioMin = 9999;
  mesMin = 12;
  diaMin = 31;
  anioMax = 0;
  mesMax = 1;
  diaMax = 1;

  string linea;
  while (numDatos < MAX_DATOS && getline(archivo, linea))
  {
    istringstream iss(linea);
    string fecha;

    if (!(iss >> fecha >> datos[numDatos].temperatura >>
          datos[numDatos].humedad >> datos[numDatos].velocidadViento >>
          datos[numDatos].presionAtmosferica))
    {
      cerr << "Error al leer los datos en la línea " << numDatos + 2 << endl;
      continue;
    }

    size_t pos1 = fecha.find('-');
    size_t pos2 = fecha.rfind('-');
    if (pos1 != string::npos && pos2 != string::npos && pos1 != pos2)
    {
      datos[numDatos].anio = stoi(fecha.substr(0, pos1)); // Año
      datos[numDatos].mes =
          stoi(fecha.substr(pos1 + 1, pos2 - pos1 - 1));  // Mes
      datos[numDatos].dia = stoi(fecha.substr(pos2 + 1)); // Día

      if (datos[numDatos].anio < anioMin ||
          (datos[numDatos].anio == anioMin && datos[numDatos].mes < mesMin) ||
          (datos[numDatos].anio == anioMin && datos[numDatos].mes == mesMin &&
           datos[numDatos].dia < diaMin))
      {
        anioMin = datos[numDatos].anio;
        mesMin = datos[numDatos].mes;
        diaMin = datos[numDatos].dia;
      }
      if (datos[numDatos].anio > anioMax ||
          (datos[numDatos].anio == anioMax && datos[numDatos].mes > mesMax) ||
          (datos[numDatos].anio == anioMax && datos[numDatos].mes == mesMax &&
           datos[numDatos].dia > diaMax))
      {
        anioMax = datos[numDatos].anio;
        mesMax = datos[numDatos].mes;
        diaMax = datos[numDatos].dia;
      }
    }
    else
    {
      cerr << "Error al procesar la fecha en la línea " << numDatos + 2 << endl;
      continue;
    }

    getline(
        iss,
        datos[numDatos]
            .condicionMeteorologica); // De esta manera se lee la línea completa
                                      // y termina en la condición metereológica
    if (!datos[numDatos].condicionMeteorologica.empty() &&
        datos[numDatos].condicionMeteorologica[0] == ' ')
    {
      datos[numDatos].condicionMeteorologica =
          datos[numDatos].condicionMeteorologica.substr(1);
    }

    numDatos++;
  }

  archivo.close();
  return 0;
}
//--------------------------------------------------------------------------------------------------------------------//
// Blindaje para verificar que la fecha exista dentro del rango
bool fechaValida(int anio, int mes, int dia, int anioMin, int mesMin,
                 int diaMin, int anioMax, int mesMax, int diaMax)
{
  if (anio < anioMin || anio > anioMax)
    return false;
  if (anio == anioMin && (mes < mesMin || (mes == mesMin && dia < diaMin)))
    return false;
  if (anio == anioMax && (mes > mesMax || (mes == mesMax && dia > diaMax)))
    return false;
  return true;
}
//--------------------------------------------------------------------------------------------------------------------//
// Blindaje para verificar que la fecha exacta exista
bool fechaExiste(int anio, int mes, int dia, const DatosMeteorologicos datos[],
                 int numDatos)
{
  for (int i = 0; i < numDatos; ++i)
  {
    if (datos[i].anio == anio && datos[i].mes == mes && datos[i].dia == dia)
    {
      return true;
    }
  }
  return false;
}
//-------------------------------------------------------------------------------------------------------------------//
// Función para encontrar rangos y verificar que las fechas ingresadas existan
// en la base de datos
void encontrarRangos(int &anio1, int &mes1, int &dia1, int &anio2, int &mes2,
                     int &dia2, int anioMin, int mesMin, int diaMin,
                     int anioMax, int mesMax, int diaMax,
                     const DatosMeteorologicos datos[], int numDatos)
{
  bool fechasValidas = false;

  while (!fechasValidas)
  {
    cout << "Digite la primera fecha (en el formato AAAA MM DD): ";
    cin >> anio1 >> mes1 >> dia1;

    cout << "Digite la segunda fecha (en el formato AAAA MM DD): ";
    cin >> anio2 >> mes2 >> dia2;
    cout << endl;

    if (fechaValida(anio1, mes1, dia1, anioMin, mesMin, diaMin, anioMax, mesMax,
                    diaMax) &&
        fechaValida(anio2, mes2, dia2, anioMin, mesMin, diaMin, anioMax, mesMax,
                    diaMax) &&
        (anio1 < anio2 ||
         (anio1 == anio2 && (mes1 < mes2 || (mes1 == mes2 && dia1 <= dia2)))) &&
        fechaExiste(anio1, mes1, dia1, datos, numDatos) &&
        fechaExiste(anio2, mes2, dia2, datos,
                    numDatos))
    { // Verifica que la fecha sea valida en el rango
      // y que exista en la base de datos
      fechasValidas = true;
    }

    // Dado el caso que no, itera en un bucle
    if (!fechasValidas)
    {
      cout << "Las fechas ingresadas están fuera del rango de datos "
              "disponibles ("
           << anioMin << "-" << mesMin << "-" << diaMin << " a " << anioMax
           << "-" << mesMax << "-" << diaMax
           << ") o no existen en la base de datos. Por favor, ingrese "
              "nuevamente."
           << endl
           << endl;
    }
  }
}
//----------------------------------------------------------------------------------------------------------//
// Verifica que las fechas ingresadas existan en la base de datos desde la ficha
// minima y maxima de la base de datos
bool fechaEnRango(int anio, int mes, int dia, int anioInicio, int mesInicio,
                  int diaInicio, int anioFin, int mesFin, int diaFin)
{
  if (anio < anioInicio || anio > anioFin)
    return false;
  if (anio == anioInicio &&
      (mes < mesInicio || (mes == mesInicio && dia < diaInicio)))
    return false;
  if (anio == anioFin && (mes > mesFin || (mes == mesFin && dia > diaFin)))
    return false;
  return true;
}

//--------------------------------------------------------------------------------------------------------//
struct Usuario {
    string usuario;
    string contrasena;
};

const int MAX_USUARIOS = 100.000;
//-------------------------------------------------------------------------------------------------------//
void salidaTxt(const Usuario &usuario) {
    ofstream salida("salida.txt");
    if (salida.is_open()) {
        salida << "RESULTADOS DEL ANÁLISIS METEREOLÓGICO:\n";
        salida << ">>\n\nUsuario que hace el reporte: " << usuario.usuario << "\n\n";
    } else {
        cerr << "No se pudo abrir el archivo salida.txt" << endl;
    }
}
//-------------------------------------------------------------------------------------------------------//
bool validarCorreoElectronico(const string &correo) {
    const regex formatoCorreo(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex_match(correo, formatoCorreo);
}
//------------------------------------------------------------------------------------------------------//
void registrarUsuario(Usuario usuarios[], int &numUsuarios, const string &nombreArchivo) {
    ofstream salida("salida.txt", ios::app);
    if (numUsuarios >= MAX_USUARIOS) {
        cout << "🚩🚩No es posible registrar más usuarios. El límite ha sido alcanzado." << endl;
        return;
    }
    Usuario nuevoUsuario;
    cout << "Ingrese nombre de correo electrónico: ";
    cin >> nuevoUsuario.usuario;

    if (!validarCorreoElectronico(nuevoUsuario.usuario)) {
        cout << "😡🤬El correo electrónico ingresado no tiene un formato válido." << endl;
        return;
    }

    cout << "Ingrese contrasena: ";
    cin >> nuevoUsuario.contrasena;

    for (int i = 0; i < numUsuarios; ++i) {
        if (usuarios[i].usuario == nuevoUsuario.usuario) {
            cout << "😒El correo de usuario ya existe. Por favor, elija otro." << endl;
            return;
        }
    }

    usuarios[numUsuarios++] = nuevoUsuario;
    cout << "✌Usuario registrado exitosamente." << endl;
    salida << "Usuario nuevo : " << nuevoUsuario.usuario << "\n\n";

    ofstream archivo(nombreArchivo, ios::binary);
    if (archivo.is_open()) {
        archivo.write(reinterpret_cast<const char *>(&numUsuarios), sizeof(int));
        for (int i = 0; i < numUsuarios; ++i) {
            archivo.write(reinterpret_cast<const char *>(&usuarios[i]), sizeof(Usuario));
        }
        archivo.close();
    } else {
        cout << "Error al abrir el archivo " << nombreArchivo << endl;
    }
}
//---------------------------------------------------------------------------------------------------//
void cargarUsuarios(Usuario usuarios[], int &numUsuarios, const string &nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (archivo.is_open()) {
        archivo.read(reinterpret_cast<char *>(&numUsuarios), sizeof(int));
        for (int i = 0; i < numUsuarios; ++i) {
            archivo.read(reinterpret_cast<char *>(&usuarios[i]), sizeof(Usuario));
        }
        archivo.close();
    } else {
        cout << "El archivo " << nombreArchivo << " no existe. Se creará uno nuevo." << endl;
        ofstream newFile(nombreArchivo, ios::binary);
        newFile.close();
    }
}
//--------------------------------------------------------------------------------------------------//
void mostrarUsuarios(const Usuario usuarios[], int numUsuarios) {
    ofstream salida("salida.txt", ios::app);
    if (numUsuarios == 0) {
        salida << "☹ No hay usuarios registrados." << endl;
    } else {
        salida << "Lista de usuarios registrados :" << endl;
        salida << "!------------------------!" << endl;
        for (int i = 0; i < numUsuarios; ++i) {
            salida << "Usuario " << i + 1 << ": " << usuarios[i].usuario << endl;
            salida << "------------------------" << endl;
        }
    }
}
//--------------------------------------------------------------------------------------------------//
bool iniciarSesion(const Usuario usuarios[], int numUsuarios) {
    ofstream salida("salida.txt", ios::app);
    string nombreUsuario, contrasena;
    cout << "Ingrese nombre de usuario: ";
    cin >> nombreUsuario;
    cout << "Ingrese contrasena: ";
    cin.ignore();
    getline(cin, contrasena);

    for (int i = 0; i < numUsuarios; ++i) {
        if (usuarios[i].usuario == nombreUsuario && usuarios[i].contrasena == contrasena) {
            cout << endl;
            salida << "Inicio de sesión exitoso. Bienvenido, ¡🖐🤙" << nombreUsuario << "!🖐🤙" << endl;
            return true;
        }
    }
    salida << "⚠ Nombre de usuario o contrasena incorrectos. Por favor, inténtelo de nuevo.⚠" << endl;
    return false;
}
//-------------------------------------------------------------------------------------------------------//
void eliminarUsuario(Usuario usuarios[], int &numUsuarios, const string &nombreArchivo) {
    ofstream salida("salida.txt", ios::app);

    string nombreUsuario;
    cout << "Ingrese nombre de usuario a eliminar: ";
    cin >> nombreUsuario;

    int index = -1;
    for (int i = 0; i < numUsuarios; ++i) {
        if (usuarios[i].usuario == nombreUsuario) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        cout << "Usuario no encontrado." << endl;
        return;
    }

    for (int i = index; i < numUsuarios - 1; ++i) {
        usuarios[i] = usuarios[i + 1];
    }
    numUsuarios--;

    salida << "Usuario eliminado 👉☠☠☠ " <<nombreUsuario<< endl;

    ofstream archivo(nombreArchivo, ios::binary | ios::trunc);
    if (archivo.is_open()) {
        archivo.write(reinterpret_cast<const char *>(&numUsuarios), sizeof(int));
        for (int i = 0; i < numUsuarios; ++i) {
            archivo.write(reinterpret_cast<const char *>(&usuarios[i]), sizeof(Usuario));
        }
        archivo.close();
    } else {
        cout << "Error al abrir el archivo " << nombreArchivo << endl;
    }
}



//-----------------------------------------------------------------------------------------------------------/////



// Función para mostrar todos los datos meteorológicos, no se llama como tal
// pero dado el caso que se necesite se instaura
void imprimirDatosMeteorologicos(const DatosMeteorologicos datos[],
                                 int numDatos)
{

  for (int i = 0; i < numDatos; ++i)
  {
    cout << datos[i].anio << "-"
         << (datos[i].mes < 10
                 ? "0"
                 : "") // El ? es para agregar un 0 si es menor a 10 la fecha
         << datos[i].mes << "-" << (datos[i].dia < 10 ? "0" : "")
         << datos[i].dia << setfill(' ') << setw(15)
         << // El setfill(' ') es para que no se llenen de 0s los espacios dados
            // por el setw
        datos[i].temperatura << setw(10) << datos[i].humedad << setw(15)
         << datos[i].velocidadViento << setw(20) << datos[i].presionAtmosferica
         << setw(15) << datos[i].condicionMeteorologica << endl;
  }
}

//-----------------------------------------------------------------------------------------------------//
// Función para hallar la temperatura máxima en un rango de fechas
void encontrarMaximaTemperatura(const DatosMeteorologicos datos[], int numDatos,
                                int anioMin, int mesMin, int diaMin,
                                int anioMax, int mesMax, int diaMax)
{
  int anio1, mes1, dia1;
  int anio2, mes2, dia2;

  encontrarRangos(anio1, mes1, dia1, anio2, mes2, dia2, anioMin, mesMin, diaMin,
                  anioMax, mesMax, diaMax, datos, numDatos);

  float tempMax = -9999; // Se declara un valor muy bajo para que vaya cambiando
                         // en el programa la temperatura máxima
  bool encontrada = false;
  int anioFinal = 0;
  int mesFinal = 0;
  int diaFinal = 0;

  cout << "Proceso realizado exitosamente y añadido al archivo salida.txt";
  for (int i = 0; i < numDatos; ++i)
  {
    if (fechaEnRango(datos[i].anio, datos[i].mes, datos[i].dia, anio1, mes1,
                     dia1, anio2, mes2, dia2))
    {
      if (datos[i].temperatura > tempMax)
      {
        tempMax = datos[i].temperatura;
        anioFinal = datos[i].anio;
        mesFinal = datos[i].mes;
        diaFinal = datos[i].dia;
        encontrada = true; // Halla la temperatura máxima
      }
    }
  }
  ofstream salida("salida.txt", ios::app);
  if (encontrada)
  {
    salida << "2) --> La temperatura máxima en el rango de fechas especificado "
              "es: "
           << tempMax << " C° en " << anioFinal << "-" << mesFinal << "-"
           << diaFinal << endl
           << endl;
  }
  else
  {
    salida << "2) --> No hay datos para el rango de fechas especificado."
           << endl
           << endl;
  }
  salida.close();
}

//--------------------------------------------------------------------------------------------------//
// Función para hallar la temperatura mínima en un rango de fechas
void encontrarMinimaTemperatura(const DatosMeteorologicos datos[], int numDatos,
                                int anioMin, int mesMin, int diaMin,
                                int anioMax, int mesMax, int diaMax)
{
  int anio1, mes1, dia1;
  int anio2, mes2, dia2;

  encontrarRangos(anio1, mes1, dia1, anio2, mes2, dia2, anioMin, mesMin, diaMin,
                  anioMax, mesMax, diaMax, datos, numDatos);

  float tempMin = 9999; // Se declara un valor muy alto para que vaya cambiando
                        // en el programa la temperatura mínima
  bool encontrada = false;
  int anioFinal = 0;
  int mesFinal = 0;
  int diaFinal = 0;

  cout << "Proceso realizado exitosamente y añadido al archivo salida.txt";
  for (int i = 0; i < numDatos; ++i)
  {
    if (fechaEnRango(datos[i].anio, datos[i].mes, datos[i].dia, anio1, mes1,
                     dia1, anio2, mes2, dia2))
    {
      if (datos[i].temperatura < tempMin)
      {
        tempMin = datos[i].temperatura;
        anioFinal = datos[i].anio;
        mesFinal = datos[i].mes;
        diaFinal = datos[i].dia;
        encontrada = true; // Halla la temperatura mínima
      }
    }
  }
  ofstream salida("salida.txt", ios::app);
  if (encontrada)
  {
    salida << "3) --> La temperatura mínima en el rango de fechas especificado "
              "es: "
           << tempMin << " C° en " << anioFinal << "-" << mesFinal << "-"
           << diaFinal << endl;
  }
  else
  {
    salida << "3) --> No hay datos para el rango de fechas especificado."
           << endl;
  }
  salida.close();
}

//------------------------------------------------------------------------------------------------------//
// Función para calcular el promedio de temperatura de un mes en específico
void calcularPromedioTemperaturaMes(const DatosMeteorologicos datos[],
                                    int numDatos)
{
  int mes, anio;
  cout << "Ingrese el mes y año (Formato MM AAAA): ";
  cin >> mes >> anio;

  float totalTemperatura = 0;
  int cantidadMediciones = 0;

  cout << "Proceso realizado exitosamente y añadido al archivo salida.txt";
  for (int i = 0; i < numDatos; ++i)
  {
    if (datos[i].anio == anio && datos[i].mes == mes)
    {
      totalTemperatura += datos[i].temperatura;
      cantidadMediciones++;
    }
  }
  ofstream salida("salida.txt", ios::app);
  if (cantidadMediciones > 0)
  {
    float promedioTemperatura = totalTemperatura / cantidadMediciones;
    salida
        << "4) --> El promedio de temperatura para " << mes << "/" << anio
        << " es: "
        << fixed // Se necesita para que el promedio se muestre con 2 decimales
        << setprecision(2) << promedioTemperatura << " C°." << endl;
  }
  else
  {
    salida << endl
           << " 4) --> El mes y año no se encuentran en la base de datos."
           << endl;
  }
  salida.close();
}

//---------------------------------------------------------------------------------------------//
// Función para calcular el promedio de temperatura de un día en específico
void calcularMayorPromedioHumedad(const DatosMeteorologicos datos[],
                                  int numDatos)
{
  string condicion;
  cout << "Ingrese la condición meteorológica (Lluvioso, Soleado, Nublado): ";
  cin.ignore(); // Ignorar el salto de línea anterior
  getline(cin, condicion);

  int mes, anio;
  cout << "Ingrese el mes y año (Formato MM AAAA): ";
  cin >> mes >> anio;

  int totalHumedad = 0;
  int cantidadMediciones = 0;
  bool encontrado = false;
  float mayorPromedio = -1.0; // Inicializar con un valor muy bajo

  cout << "Proceso realizado exitosamente y añadido al archivo salida.txt";
  for (int i = 0; i < numDatos; ++i)
  {
    if (datos[i].condicionMeteorologica == condicion && datos[i].mes == mes &&
        datos[i].anio == anio)
    {
      totalHumedad += datos[i].humedad;
      cantidadMediciones++;
      encontrado = true;
    }
  }
  ofstream salida("salida.txt", ios::app);
  if (!encontrado)
  {
    salida << "5) --> No hay datos para la condición meteorológica, mes y año "
              "especificados (recuerda que las condiciones deben estar como se "
              "muestra al principio)"
           << endl;
    return;
  }
  // Calcular el promedio de humedad (utilizamos static_cast<float> para no
  // ignorar los 0's del decimal)
  float promedio = static_cast<float>(totalHumedad) / cantidadMediciones;
  if (promedio > mayorPromedio)
    mayorPromedio = promedio;

  salida << "5) --> El mayor promedio de humedad para la condición "
         << condicion << " en el mes " << mes << "/" << anio << " es: "
         << fixed // Se necesita para que el promedio se muestre con 2 decimales
         << setprecision(2) << mayorPromedio << " %" << endl;
  salida.close();
}

//---------------------------------------------------------------------------------------------//
void calcularMenorPromedioHumedad(const DatosMeteorologicos datos[],
                                  int numDatos)
{
  string condicion;
  cout << "Ingrese la condición meteorológica (Lluvioso, Soleado, Nublado): ";
  cin.ignore(); // Se salta a la siguiente linea si no se pone
  getline(cin, condicion);

  int mes, anio;
  cout << "Ingrese el mes y año (Formato MM AAAA): ";
  cin >> mes >> anio;

  int totalHumedad = 0;
  int cantidadMediciones = 0;
  bool encontrado = false;
  float menorPromedio = 9999.0; // Inicializar con un valor muy alto

  cout << "Proceso realizado exitosamente y añadido al archivo salida.txt";
  for (int i = 0; i < numDatos; ++i)
  {
    if (datos[i].condicionMeteorologica == condicion && datos[i].mes == mes &&
        datos[i].anio == anio)
    {
      totalHumedad += datos[i].humedad;
      cantidadMediciones++;
      encontrado = true;
    }
  }
  ofstream salida("salida.txt", ios::app);
  if (!encontrado)
  {
    salida << "6) --> No hay datos para la condición meteorológica, mes y año "
              "especificados (recuerda que las condiciones deben estar como se "
              "muestra al principio)"
           << endl;
    return;
  }
  // Calcular el promedio de humedad (utilizamos static_cast<float> para no
  // ignorar los 0's del decimal)
  float promedio = static_cast<float>(totalHumedad) / cantidadMediciones;
  if (promedio < menorPromedio)
    menorPromedio = promedio;

  salida << "6) -- > El menor promedio de humedad para la condición "
         << condicion << " en el mes " << mes << "/" << anio << " es: "
         << fixed // Se necesita para que el promedio se muestre con 2 decimales
         << setprecision(2) << menorPromedio << " %" << endl;
  salida.close();
}

//-----------------------------------------------------------------------------------------------//
void mostrarDatosMeteorologicosPorRango(const DatosMeteorologicos datos[],
                                        int numDatos, int anioMin, int mesMin,
                                        int diaMin, int anioMax, int mesMax,
                                        int diaMax)
{
  int anio1, mes1, dia1;
  int anio2, mes2, dia2;

  ofstream salida("salida.txt", ios::app);
  encontrarRangos(anio1, mes1, dia1, anio2, mes2, dia2, anioMin, mesMin, diaMin,
                  anioMax, mesMax, diaMax, datos, numDatos);
  cout << "Proceso realizado exitosamente y añadido al archivo salida.txt";
  salida << "1) --> Los datos meteorológicos en el rango de fechas "
            "especificado son: "
         << endl;
  salida << "Fecha            "
         << "Temperatura        "
         << "Humedad     "
         << "Vel. Viento     "
         << "Presión Atmosférica   "
         << "Cond. Meteor." << endl;

  for (int i = 0; i < numDatos; i++)
  {
    if (fechaEnRango(datos[i].anio, datos[i].mes, datos[i].dia, anio1, mes1,
                     dia1, anio2, mes2, dia2))
    {
      salida
          << datos[i].anio << "-"
          << (datos[i].mes < 10
                  ? "0"
                  : "") // El ? es para agregar un 0 si es menor a 10 la fecha
          << datos[i].mes << "-" << (datos[i].dia < 10 ? "0" : "")
          << datos[i].dia << setfill(' ') << setw(15)
          << // El setfill(' ') es para que no se llenen de 0s los espacios
             // dados por el setw
          datos[i].temperatura << setw(15) << datos[i].humedad << setw(15)
          << datos[i].velocidadViento << setw(20) << datos[i].presionAtmosferica
          << setw(20) << datos[i].condicionMeteorologica << endl;
    }
  }
  salida.close();
}


//----------------------------------------------------------------------------------------//
void probabilidadIncendio(const DatosMeteorologicos datos[], int numDatos)
{
  int anio, mes, dia;
  bool fechaValida = false;

  cout << "Ingrese la fecha (en el formato AAAA MM DD): " << endl;
  cin >> anio >> mes >> dia;

  ofstream salida("salida.txt", ios::app);
  if (!salida.is_open())
  {
    cerr << "Error al abrir el archivo salida.txt" << endl;
    return;
  }

  for (int i = 0; i < numDatos; ++i)
  {
    if (datos[i].anio == anio && datos[i].mes == mes && datos[i].dia == dia)
    {
      fechaValida = true;
      salida << "7) --> Probabilidad de incendio: " << endl
             << "Temperatura: " << datos[i].temperatura << "°C - "
             << "Velocidad del viento: " << datos[i].velocidadViento << " km/h - "
             << "Condición meteorológica: " << datos[i].condicionMeteorologica << endl;

      if (datos[i].temperatura > 30 &&
          datos[i].condicionMeteorologica == "Soleada" &&
          datos[i].velocidadViento >= 7 && datos[i].velocidadViento <= 10)
      {
        salida << "Hay una alta probabilidad de incendio." << endl;
      }
      else if (datos[i].temperatura >= 25 && datos[i].temperatura <= 30 &&
               datos[i].velocidadViento >= 3 &&
               datos[i].velocidadViento <= 5)
      {
        salida << "Hay una probabilidad de incendio debido a temperatura ligeramente alta." << endl;
      }
      else if (datos[i].temperatura >= 15 && datos[i].temperatura <= 20 &&
               datos[i].velocidadViento < 2)
      {
        salida << "No hay probabilidad de incendio." << endl;
      }
      else
      {
        salida << "La probabilidad de incendio es baja." << endl;
      }
      break; // Salimos del bucle después de encontrar la primera coincidencia
    }
  }

  salida.close();

  if (!fechaValida)
  {
    cout << "No hay datos para la fecha especificada." << endl;
  }
  else
  {
    cout << "Proceso realizado exitosamente y añadido al archivo salida.txt" << endl;
  }
}


//-------------------------------------------------------------------------------------------------//
void probabilidadTormenta(const DatosMeteorologicos datos[], int numDatos)
{
  int anio, mes, dia;
  bool fechaValidas = false;

  cout << "Ingrese la fecha (en el formato AAAA MM DD): " << endl;
  cin >> anio >> mes >> dia;

  ofstream salida("salida.txt", ios::app);
  if (!salida.is_open())
  {
    cerr << "Error al abrir el archivo salida.txt" << endl;
    return;
  }

  for (int i = 0; i < numDatos; ++i)
  {
    if (datos[i].anio == anio && datos[i].mes == mes && datos[i].dia == dia)
    {
      fechaValidas = true;
      salida << "8) --> Probabilidad de tormenta o lluvia: " << endl
             << "Presión Atmosférica: " << datos[i].presionAtmosferica << "hPa - "
             << "Húmedad: " << datos[i].humedad << "% - "
             << "Condición meteorológica: " << datos[i].condicionMeteorologica << endl;

      if (datos[i].presionAtmosferica > 1010 &&
          datos[i].condicionMeteorologica == "Lluvioso" &&
          datos[i].humedad > 70)
      {
        salida << "La probabilidad de tormenta es alta" << endl;
      }
      else if (datos[i].presionAtmosferica < 1010 &&
               datos[i].condicionMeteorologica == "Lluvioso" &&
               datos[i].humedad > 70)
      {
        salida << "La probabilidad de lluvia es alta" << endl;
      }
      else if (datos[i].presionAtmosferica == 1010 &&
               datos[i].condicionMeteorologica == "Lluvioso" &&
               datos[i].humedad > 70)
      {
        salida << "No se puede determinar la probabilidad de lluvia o tormenta con los datos proporcionados de esta fecha." << endl;
      }
      else
      {
        salida << "La probabilidad de tormenta o lluvia es baja" << endl;
      }
      break; // Salimos del bucle después de encontrar la primera coincidencia
    }
  }

  salida.close();

  if (!fechaValidas)
  {
    cout << "No hay datos para la fecha especificada." << endl;
  }
  else
  {
    cout << "Proceso realizado exitosamente y añadido al archivo salida.txt" << endl;
  }
}


//-------------------------------------------------------------------------------------------------//
void agregarDatosMeteorologicos(DatosMeteorologicos datos[], int &numDatos,
                                int anioMin, int mesMin, int diaMin,
                                int anioMax, int mesMax, int diaMax)
{

  if (numDatos >= MAX_DATOS)
  {
    cout << "No se pueden agregar más datos, se ha alcanzado el máximo "
            "permitido."
         << endl;
    return;
  }

  DatosMeteorologicos nuevoDato;

  cout << "Ingrese la fecha (AAAA MM DD): ";
  cin >> nuevoDato.anio >> nuevoDato.mes >> nuevoDato.dia;

  // Buscar si ya existe una entrada con la misma fecha
  int indiceExistente = -1;
  for (int i = 0; i < numDatos; ++i)
  {
    if (datos[i].anio == nuevoDato.anio && datos[i].mes == nuevoDato.mes &&
        datos[i].dia == nuevoDato.dia)
    {
      indiceExistente = i;
      break;
    }
  }

  if (indiceExistente != -1)
  {
    // Si ya existe una entrada con la misma fecha, actualizar los datos
    cout << "Ya existe una entrada para la fecha especificada. Se actualizarán "
            "los datos existentes."
         << endl
         << endl;
    cout << "Ingrese la temperatura: ";
    cin >> datos[indiceExistente].temperatura;
    cout << "Ingrese la humedad: ";
    cin >> datos[indiceExistente].humedad;
    cout << "Ingrese la velocidad del viento: ";
    cin >> datos[indiceExistente].velocidadViento;
    cout << "Ingrese la presión atmosférica: ";
    cin >> datos[indiceExistente].presionAtmosferica;
    cout << "Ingrese la condición meteorológica: ";
    cin.ignore(); // Ignorar el salto de línea anterior
    getline(cin, datos[indiceExistente]
                     .condicionMeteorologica); // Leer la línea completa
  }
  else
  {
    // Si no existe una entrada con la misma fecha, agregar una nueva entrada
    cout << "Ingrese la temperatura: ";
    cin >> nuevoDato.temperatura;
    cout << "Ingrese la humedad: ";
    cin >> nuevoDato.humedad;
    cout << "Ingrese la velocidad del viento: ";
    cin >> nuevoDato.velocidadViento;
    cout << "Ingrese la presión atmosférica: ";
    cin >> nuevoDato.presionAtmosferica;
    cout << "Ingrese la condición meteorológica: ";
    cin.ignore();                                   // Ignorar el salto de línea anterior
    getline(cin, nuevoDato.condicionMeteorologica); // Leer la línea completa

    // Agregar el nuevo dato al final del arreglo
    datos[numDatos] = nuevoDato;
    numDatos++;
  }

  // Escribir todos los datos en el archivo
  ofstream salida("salida.txt", ios::app);
  ofstream archivoSalida(nombreArchivo);
  if (archivoSalida.is_open())
  {
    archivoSalida
        << "Fecha Temperatura (C) Humedad (porc.) Velocidad del Viento (km/h) "
           "Presión Atmosférica (hPa) Condición Meteorológica"
        << endl;
    for (int i = 0; i < numDatos; ++i)
    {
      archivoSalida << datos[i].anio << "-" << (datos[i].mes < 10 ? "0" : "")
                    << datos[i].mes << "-" << (datos[i].dia < 10 ? "0" : "")
                    << datos[i].dia << " " << datos[i].temperatura << " "
                    << datos[i].humedad << " " << datos[i].velocidadViento
                    << " " << datos[i].presionAtmosferica << " "
                    << datos[i].condicionMeteorologica << endl;
    }
    cout << "Proceso realizado exitosamente y añadido al archivo salida.txt";
    salida << endl
           << "9) --> Datos meteorológicos agregados exitosamente." << endl;
    archivoSalida.close();
  }
  else
  {
    cout << "Error al abrir el archivo para escritura." << endl;
  }
}



//--------------------------------------------------------------------------------------------------------//
int menuDatosMeteorologicos()
{
  int opcion;
  cout << "|-------------------------------------------------------------------"
          "-------|\n";
  cout << "|                         MENÚ DATOS METEOROLOGICOS                 "
          "       |\n";
  cout << "|-------------------------------------------------------------------"
          "-------|\n";

  cout << endl
       << endl;

  cout << "|-------------------------------------------------------------------"
          "-------|\n";
  cout << "|          1. Mostrar datos meteorológicos por rango de fechas      "
          "       |\n";
  cout << "|-------------------------------------------------------------------"
          "-------|\n";
  cout << "|         2. Encontrar temperatura máxima en un rango de fechas     "
          "       |\n";
  cout << "|-------------------------------------------------------------------"
          "-------|\n";
  cout << "|         3. Encontrar temperatura mínima en un rango de fechas     "
          "       |\n";
  cout << "|-------------------------------------------------------------------"
          "-------|\n";
  cout << "|       4. Calcular promedio de temperatura en un mes especificado  "
          "       |\n";
  cout << "|-------------------------------------------------------------------"
          "-------|\n";
  cout << "|         5. Calcular mayor promedio de humedad con relación a      "
          "       |\n";
  cout << "|                      una condición meteorológica                  "
          "       |\n";
  cout << "|-------------------------------------------------------------------"
          "-------|\n";
  cout << "|         6. Calcular menor promedio de humedad con relación a      "
          "       |\n";
  cout << "|                      una condición meteorológica                  "
          "       |\n";
  cout << "|-------------------------------------------------------------------"
          "-------|\n";
  cout << "|  7. Calcular la probabilidad de incendio en relación a una "
          "temperatura,  |\n";
  cout << "|              velocidad del viento y condición meteorológica       "
          "       |\n";
  cout << "|-------------------------------------------------------------------"
          "-------|\n";
  cout << "|  8. Calcular la probabilidad de una tormenta en relación a una "
          "presión   | \n";
  cout << "|             atmosférica, humedad y condición meteorológica        "
          "       |\n";
  cout << "|-------------------------------------------------------------------"
          "-------|\n";
  cout << "|                     9. Agregar datos meteorológicos               "
          "       |\n";
  cout << "|-------------------------------------------------------------------"
          "-------|\n";
  cout << "|--------|\n------------------------10.Cerrar sesion-------------------------"
          "-----------|\n";

  cout << endl
       << endl
       << "Ingrese su opción: ";
  cin >> opcion;

  return opcion;
}


//-----------------------------------------------------------------------------------------------//
bool continuarPrograma()
{
  ofstream salida("salida.txt", ios::app);

  char respuesta;
  while (true)
  {
    cout << endl
         << "¿🧐Desea seguir operando? (s/n): ";
    cin >> respuesta;
    respuesta = tolower(respuesta); // Convertir a minúscula para hacer la
                                    // comparación y evitar errores

    if (respuesta == 's')
    {
      return true;
    }
    else if (respuesta == 'n')
    {
      cout << endl;
      salida << "¡👌👌GRACIAS POR USAR NUESTRO PROGRAMA! (👵👶que no se te olvide "
              "revisar el archivo salida.txt)"
           << endl;
      return false;
      break;
    }
    else
    {
      cout << "Respuesta no válida. Por favor, responda 's' para sí o 'n' "
              "para no."
           << endl;
    }
  }
}
//-----------------------------------------------------------------------------------------------//
int main()
{
  ofstream salida("salida.txt", ios::app);
  Usuario usuarios[MAX_USUARIOS];             // Arreglo para almacenar los usuarios registrados
  int numUsuarios = 0;                   // Número actual de usuarios registrados
  string nombreArchivo = "usuarios.bin"; // Nombre del archivo binario

  // Cargar usuarios desde el archivo binario al inicio del programa
  cargarUsuarios(usuarios, numUsuarios, nombreArchivo);

  // Menú principal de login
  int opcion;
  do
  {

    cout << endl
         << "¡👋👏BIENVENIDO AL SISTEMA DE PREVISIONES METEOROLÓGICAS DE LA IDEAM!👏👋"
         << endl
         << endl;
    cout << "\n1. Iniciar sesión" << endl;
    cout << "2. Registrar usuario" << endl;
    cout << "3. Mostrar usuarios registrados" << endl;
    cout << "4. Eliminar usuario(admin)" << endl;
    cout << "5. Salir" << endl;

    cout << "Ingrese su opción: ";
    cin >> opcion;
    cout << endl
         << endl;

    switch (opcion)
    {
    case 1:
      if (iniciarSesion(usuarios, numUsuarios))
      {
        // si el usuario esta registrado e inicia sesion, el menu se le
        // muestra------------
        DatosMeteorologicos datos[MAX_DATOS];

        int numDatos = 0;
        int anioMin, mesMin, diaMin, anioMax, mesMax, diaMax;

        if (leerDatosMeteorologicos("datos_meteorologicos.txt", datos, numDatos,
                                    anioMin, mesMin, diaMin, anioMax, mesMax,
                                    diaMax) != 0)
        {
          cout << "Error al cargar los datos meteorológicos." << endl;
          return 1;
        }

        cout << endl
             << endl
             << "Datos meteorológicos cargados correctamente." << endl;
        cout << "Fecha mínima disponible: " << anioMin << "-" << setw(2)
             << setfill('0') << mesMin << "-" << setw(2) << setfill('0')
             << diaMin << endl;
        cout << "Fecha máxima disponible: " << anioMax << "-" << setw(2)
             << setfill('0') << mesMax << "-" << setw(2) << setfill('0')
             << diaMax << endl
             << endl;

        int opcionDatosMeteorologicos;
        do
        {
          opcionDatosMeteorologicos = menuDatosMeteorologicos();
          switch (opcionDatosMeteorologicos)
          {
          case 1:
          {

            mostrarDatosMeteorologicosPorRango(datos, numDatos, anioMin, mesMin,
                                               diaMin, anioMax, mesMax, diaMax);
            break;
          }
          case 2:
          {
            encontrarMaximaTemperatura(datos, numDatos, anioMin, mesMin, diaMin,
                                       anioMax, mesMax, diaMax);
            break;
          }
          case 3:
          {
            encontrarMinimaTemperatura(datos, numDatos, anioMin, mesMin, diaMin,
                                       anioMax, mesMax, diaMax);
            break;
          }
          case 4:
          {
            calcularPromedioTemperaturaMes(datos, numDatos);
            break;
          }
          case 5:
          {
            calcularMayorPromedioHumedad(datos, numDatos);
            break;
          }
          case 6:
          {
            calcularMenorPromedioHumedad(datos, numDatos);
            break;
          }
          case 7:
          {
            probabilidadIncendio(datos, numDatos);
            break;
          }
          case 8:
          {
            probabilidadTormenta(datos, numDatos);
            break;
          }
          case 9:
          {
            agregarDatosMeteorologicos(datos, numDatos, anioMin, mesMin, diaMin,
                                       anioMax, mesMax, diaMax);
            break;
          }
          case 10:
          {
           cout << "🏃‍♂️🏃‍♂️🏃‍♀️Cerrando sesion....................." << endl;
           salida << "Cerrando sesion....................." << endl;
           salida << "🙋‍♂️🙋‍♀️Gracias por usar el programa. ¡Hasta luego!"<< endl;
           salida.close();

            main();
            return 0;
          }
          case 11:
          {
            cout << "🙋‍♂️🙋‍♀️Gracias por usar el programa. ¡Hasta luego!" << endl;
            return 0;
          }

          default:
          {
            cout << endl
                 << "OPCIÓN INVÁLIDA - INTENTELO DE NUEVO" << endl;
            break;
          }
          }
        } while (continuarPrograma());
        return 0;
      }

      ///-------------------- opciones loginnnnnn de registro y mostrar usuario--------------------------------

      break;
    case 2:
      registrarUsuario(
          usuarios, numUsuarios,
          nombreArchivo); // Llamar a la función para registrar un nuevo usuario
      break;

    case 3:
      mostrarUsuarios(usuarios,
                      numUsuarios); // Llamar a la función para mostrar todos
                                    // los usuarios registrados
      break;
    case 4:
     
      eliminarUsuario(usuarios, numUsuarios, nombreArchivo);
      main();
      break;
    case 5:
      cout << "👍👍Gracias por usar el programa. ¡Hasta luego!" << endl;

      salida << "👍👍Gracias por usar el programa. ¡Hasta luego!" << endl;
      return 0;
      break;
    default:
      cout << endl
           << "OPCIÓN INVÁLIDA - INTENTELO DE NUEVO" << endl;
    }
  } while (opcion !=
           4); // Continuar el bucle hasta que se seleccione la opción de salida

  return 0;

  //-----------------------------------------------------------------------------------------------------------
}