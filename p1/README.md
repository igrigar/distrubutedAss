# Objetivo

El objetivo de esta práctica es que el alumno llegue a conocer los principales
conceptos relacionados con la comunicación de procesos usando sockets TCP. Para
ello se propone desarrollar una aplicación de notificación de mensajes entre
usuarios conectados a Internet.

# Desarrollo del servicio

Se pretende la implementación de dos programas:

  * Un __servidor concurrente multihilo__ que proporcione el servicio de
    comunicación entre los distintos clientes registrados en el sistema,
    gestione las conexiones de los mismos y el almacenamiento de los mensajes
    enviados a un cliente no conectado en el sistema.

  * Un __cliente multihilo__ que se comunique con el servidor y sea capaz de
    enviar y recibir mensajes. Uno de los hilos se utilizará para enviar
    mensajes al servido y el otro para recibirlos.
