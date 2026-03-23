/**
 * @file middleware_client.h
 * @brief Interface du client valisette TCP.
 */
#ifndef MIDDLEWARE_CLIENT_H
#define MIDDLEWARE_CLIENT_H

/**
 * @brief Lance le client valisette et sa boucle principale.
 *
 * @param[in] host_ip Adresse IP de l'hôte.
 * @param[in] host_port Port TCP de l'hôte.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int client_run(const char *host_ip, int host_port);

#endif
