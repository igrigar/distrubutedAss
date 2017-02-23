# Práctica 1

Se desea diseñar e implementar un modelo de vector distribuido. Sobre un
vector distribuido se definen los siguientes servicios:

    * int init(char \*nombre, int N). Este servicio permite inicializar un 
    vector distribuido de N números enteros. La función devuelve 1 cuando el
    vector se ha creado por primera vez. En caso de que el vector ya esté
    creado con el mismo número de componentes, la función devuelve 0. La
    función devuelve -1 en caso de error, por ejemplo que se intente crear un
    vector que existe con un número de componentes distinto.

    * int set(char \*nombre, int i, int valor). Este servicio inserta el valor en
    la posición i del vector nombre. La función devuelve 0 en caso de éxito y -1
    en caso de error, por ejemplo, que se intente insertar un elemento en una
    posición invalida, o el vector no exista.

    * int get(char \*nombre, int i, int \*valor). Este servicio permite recuperar
    el valor del elemento i del vector nombre. La función devuelve 0 en caso de
    éxito y -1 en caso de error, por ejemplo, que se intente recuperar un
    elemento en una posición invalida, o el vector no exista.

    * int destroy(chat \*nombre). Este servicio permite borrar un vector
    previamente creado. La función devuelve 1 en caso de éxito y -1 en caso de
    error.

Diseñe e implemente, utilizando colas de mensajes POSIX, el sistema que
implemente este servicio de vectores distribuidos. Para ello debe:

    1. Desarrollar el código del servidor (**servidor.c**) encargado de gestionar
    los vectores distribuidos.

    2. Desarrollar el código que implementa los servicios anteriores (**init**,
    **set**, **get** y **destroy**). El código se desarrollará sobre el archivo
    con nombre **array.c**. Este es el código que ofrece la interfaz a los
    clientes y se encarga de implementar los servicios anteriores contactando con
    el servidor anterior. A partir de dicha implementación se deberá crear una
    biblioteca dinámica denominada **libarray.so**. Esta será la biblioteca que
    utilizarán las aplicaciones para usar el servicio. Debe investigar y buscar
    la forma de crear dicha biblioteca.

    3. Desarrollar el código de un cliente (**cliente.c**) que utilice las
    funciones anteriores. El ejecutable de este programa tiene que generarse
    empleando la biblioteca desarrollada en el apartado anterior, es decir, el
    código de este cliente debe enlazarse con la biblioteca anterior.
