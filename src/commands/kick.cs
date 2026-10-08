using System;
using System.Text;
using System.Data;
using System.Data.SQLite;

using NetCord;
using NetCord.Hosting.Gateway;
using NetCord.Hosting.Services;
using NetCord.Hosting.Services.ApplicationCommands;
using NetCord.Rest;

public class KickCommand: ApplicationCommandModule<SlashCommandContext> {
  [SlashCommand("kick", "kick a user from the server")]
  public async Task<string> Power(
      [SlashCommandParameter(Name = "user", Description = "The user to kick")] GuildUser targetUser,
      [SlashCommandParameter(Name = "reason", Description = "The reason for the kick")] string kickReason) {
        string User(User? user = null)
            {
                user ??= Context.User;
                return user.Username;
            }
        try {
            using var connection = new SqliteConnection(@"Data Source=db\database.db");
            connection.Open();
    
            Console.WriteLine("Connected to the SQLite database!");
            await targetUser.KickAsync(new RestRequestProperties().WithAuditLogReason(kickReason));
            return User() + " has kicked " + targetUser.Username + " for: " + kickReason;
        } catch (Exception e) {
            Console.WriteLine(e.Message);
            return "Failed to kick user";
        }
  }
}
