# Documentacion del parser

## Para que sirve

Este parser sirve para leer el archivo CSV del proyecto:

```text
data/WA_Fn-UseC_-Telco-Customer-Churn.csv
```

La idea es tomar los datos del archivo y guardarlos en una estructura de C++ para poder usarlos despues en los algoritmos del trabajo.

Por ahora el parser solo lee los datos. Todavia no hace MergeSort, Binary Search, Kruskal ni Knapsack.

## Archivos usados

Los archivos principales son:

```text
src/parser.hpp
src/parser.cpp
```

Tambien hay una prueba sencilla para revisar que el parser funcione desde consola.

## Estructura `Solicitud`

Cada fila del CSV se guarda en una variable de tipo `Solicitud`.

La estructura es esta:

```cpp
struct Solicitud {
    std::string customerID;
    int tenure;
    double monthlyCharges;
    double totalCharges;
    std::string churn;
};
```

Significado de cada campo:

| Campo | Que guarda |
|---|---|
| `customerID` | El ID del cliente. |
| `tenure` | Cuantos meses lleva el cliente con el servicio. |
| `monthlyCharges` | Cuanto paga el cliente cada mes. |
| `totalCharges` | Cuanto ha pagado en total. |
| `churn` | Dice si el cliente se fue o no. Normalmente es `Yes` o `No`. |

## Funcion que lee el CSV

La funcion principal es:

```cpp
std::vector<Solicitud> parseSolicitudes(const std::string& path,
                                        int& totalRegistros,
                                        int& totalChargesNulos);
```

Lo que recibe:

| Parametro | Para que sirve |
|---|---|
| `path` | Es la ruta del archivo CSV. |
| `totalRegistros` | Aqui se guarda cuantos registros se leyeron. |
| `totalChargesNulos` | Aqui se guarda cuantos registros tenian `TotalCharges` vacio. |

Lo que devuelve:

```cpp
std::vector<Solicitud>
```

O sea, devuelve una lista con todos los clientes leidos.

## Columnas que se usan

Del CSV solo se toman estas columnas:

```text
customerID
tenure
MonthlyCharges
TotalCharges
Churn
```

Las demas columnas no se usan por ahora.

## Como encuentra las columnas

El parser primero lee la primera linea del CSV, que es el header.

Con ese header busca el nombre de cada columna. Por eso no depende de que las columnas esten exactamente en una posicion fija.

Por ejemplo, si `MonthlyCharges` cambia de lugar en el CSV, igual se puede encontrar porque se busca por el nombre.

Si falta una columna importante, el programa muestra un error.

## Como separa cada linea

El parser tiene una funcion auxiliar para separar una linea del CSV en campos.

Hace esto:

- Separa usando comas.
- Quita espacios al inicio y al final.
- Tiene soporte basico para comillas dobles.

El dataset no tiene casos complicados con comas dentro de textos, pero igual se hizo un poco mas ordenado que un split simple.

## Conversion de datos

Los datos vienen como texto porque salen del CSV. Entonces se convierten asi:

| Columna del CSV | Campo en C++ | Conversion |
|---|---|---|
| `customerID` | `customerID` | Se deja como texto. |
| `tenure` | `tenure` | Se pasa a `int`. |
| `MonthlyCharges` | `monthlyCharges` | Se pasa a `double`. |
| `TotalCharges` | `totalCharges` | Se pasa a `double`. |
| `Churn` | `churn` | Se deja como texto. |

Para convertir se usan funciones normales de C++:

```cpp
std::stoi
std::stod
```

## Caso especial de `TotalCharges`

En algunos registros, `TotalCharges` viene vacio o con espacios.

Si pasa eso, el parser hace esto:

```text
totalCharges = 0.0
totalChargesNulos suma 1
```

Esto se hace para que el programa no falle al intentar convertir un campo vacio a numero.

## Conteos

`totalRegistros` cuenta cuantas filas de datos se leyeron.

No cuenta:

- El header.
- Lineas vacias.

El tamano del vector debe ser igual a `totalRegistros`.

Con el dataset actual, la salida fue:

```text
totalRegistros: 7043
totalChargesNulos: 11
tamanoVector: 7043
primerCustomerID: 7590-VHVEG
ultimoCustomerID: 3186-AJIEK
```

## Errores que pueden pasar

El parser puede fallar si:

- No encuentra el archivo.
- Falta una columna importante.
- Una fila esta incompleta.
- Un numero viene mal escrito y no se puede convertir.

## Como compilar

Se compila con:

```powershell
g++ -std=c++17 -O2 -o ada_pf src/main.cpp src/*.cpp
```

## Como probarlo

Se ejecuta asi:

```powershell
.\ada_pf.exe data\WA_Fn-UseC_-Telco-Customer-Churn.csv
```

La prueba imprime:

- Total de registros.
- Cantidad de `TotalCharges` vacios.
- Tamano del vector.
- Primer `customerID`.
- Ultimo `customerID`.

## Que esta hecho

Ya esta hecho:

- Leer el CSV.
- Leer el header.
- Buscar columnas por nombre.
- Sacar solo las columnas necesarias.
- Convertir numeros.
- Manejar `TotalCharges` vacio.
- Contar registros.
- Guardar todo en un `vector<Solicitud>`.

Falta para despues:

- MergeSort.
- Binary Search.
- Kruskal.
- Knapsack.
