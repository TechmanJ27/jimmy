#include "main.h"

#include "command.h"
#include "database.h"

#include <stdio.h>

#include <concord/discord.h>
#include <concord/logmod.h>
#include <concord/logconf.h>

#include "token.h"

#include <pthread.h>
#include <stdlib.h>

#include <unistd.h>

#define OK printf("%s\n", "\033[32mOK!\033[0m")

static pthread_mutex_t g_mutex_ready = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_cond_ready = PTHREAD_COND_INITIALIZER;
static u64snowflake g_appid = 0;

static void on_ready(struct discord *client, const struct discord_ready *event) {
  pthread_mutex_lock(&g_mutex_ready);
  g_appid = event->application->id;
  pthread_cond_signal(&g_cond_ready);
  pthread_mutex_unlock(&g_mutex_ready);
  logmod_log(INFO, NULL, "Discord bot is ready!");
  logmod_log(INFO, NULL, "Application ID: %lu", g_appid);
}

static void on_guild_create(struct discord* client, const struct discord_guild* event) {
  pthread_mutex_lock(&g_mutex_ready);

  while (g_appid == 0) {
    pthread_cond_wait(&g_cond_ready, &g_mutex_ready);
  }

  pthread_mutex_unlock(&g_mutex_ready);

  deploy_commands(client, g_appid, event->id);
}

int main(const int argc, char *argv[]) {
  printf("%s\n", "  jimmy - " PROJECT_VERSION);
  printf("%s\n", "  Made by TheMonHub");
  printf("%s\n", "  Licensed Under Apache-2.0");

  printf("%s\n", " !!! THIS IS AN INDEV BUILD\n !!! SOME FUNCTIONALITY ARE\n !!! NOT FINISHED AND MUST\n !!! NOT BE USED");

  printf("%s\n", "=============================");

  printf("%s\n", "Press ENTER to continue...");
  getchar();

  char token[TOKEN_MAX_SIZE];

  printf("%s\n", "Reading token from a file...");
  int read_status = read_token(argc, argv, token, sizeof(token));
  if (read_status != 0) {
    fprintf(stderr, "%s", "\033[31mFATAL\033[0m - failed to read token from a file!\n");
    return read_status;
  }
  OK;

  printf("%s\n", "Reading database...");
  int database_status = database_init(argc, argv);
  if (database_status != 0) {
    fprintf(stderr, "%s", "\033[31mERROR\033[0m - failed to initialize database!\n");
    return database_status;
  }
  OK;

  printf("%s\n", "Initializing a discord bot...");

  srand(time(NULL));

  struct discord *client = discord_init(token);

  logconf_set_level(discord_get_logconf(client), LOG_INFO);

  if (client == NULL) {
    fprintf(stderr, "%s", "\033[31mFATAL\033[0m - failed to initialize a Discord bot!\n");
    database_fini();
    discord_cleanup(client);
    return 2;
  }
  logmod_log(INFO, NULL, "\033[32mSUCCESS\033[0m - Initialized a Discord bot!");

  logmod_log(INFO, NULL, "Setting callback functions...");
  discord_set_on_ready(client, &on_ready);
  discord_set_on_interaction_create(client, &on_interaction);
  discord_set_on_guild_create(client, &on_guild_create);

  logmod_log(INFO, NULL, "Running a discord bot...");

  CCORDcode discord_status = CCORD_OK;

  discord_status = discord_run(client);
  if (discord_status != CCORD_OK) {
    logmod_log(ERROR, NULL, "An error occurred during discord run!");
  }

  logmod_log(INFO, NULL, "The discord bot has been stopped! Cleaning up...");

  database_fini();
  discord_cleanup(client);
  OK;
  return discord_status;
}
