# IRC_ES

`drestep` `mmartine` `igvisera`

## Description

Estamos replicando el funcionamiento de Internet Relay Chat (IRC) utilizando **C++98**. El servidor debe ser capaz de gestionar múltiples clientes de forma simultánea y sin bloqueos (non-blocking)

### Descripción tecnica

`ft_irc` es un servidor basado en eventos que utiliza `epoll` para la conexiones multiples de entrada/salida (I/O). Permite a los usuarios conectarse tanto a través de clientes IRC estándar (HexChat) o mediante herramientas de conexión TCP sin procesar, como `nc` (Netcat). Una vez conectados, los usuarios pueden registrarse en la red, cambiar su apodo (nickname), unirse a canales de chat y enviar mensajes privados o globales a otros usuarios en tiempo real.

## Instructions

Terminal del servidor

```c
make && ./ircsev 5555 passwd
```

Terminal del cliente

```c
nc localhost 5555
```

| Command | Description |
| --- | --- |
| PASS \<password> | Ingresamos la contraseña para poder acceder al servidor. |
| USER \<username> 0 * \<realname> | Registra tu nombre de usuario y nombre real en el sistema. |
| NICK \<nickname> | Asignamos que nickname tendremos. |
| PRIVMSG \<receptor> \<mensaje> | Envía un mensaje privado a un usuario o a todo un canal. |
| JOIN \<canal>[,\<canal>] | Te permite unirte a uno o varios canales |
| INVITE \<usuario> \<canal> | Invita a un usuario a un canal (necesario si el canal es `+i`). |
| TOPIC \<canal> [:\<topic>] | Consulta o cambia el tema de conversación del canal. |
| KICK \<canal> \<usuario> [:\<razón>] | Consulta o cambia el tema de conversación del canal. |
| PART \<canal>[,\<canal>] [:\<mensaje>] | Sales de un canal específico, opcionalmente con un mensaje de despedida. |
| MODE \<canal/user> \<modos> [args] | Cambia permisos o estados. |
| NAMES [\<canal>] | Lista los apodos de todos los usuarios conectados en un canal |
| QUIT [:\<mensaje>] | Cierra tu conexión con el servidor definitivamente. |

## Resources

Documentación de [ircdoc](https://modern.ircdocs.horse), gemini para errores de compilación o en el funcionamiento del código