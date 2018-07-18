#r "System.Configuration"
#r "System.Data"
#r "Newtonsoft.Json"
using System;
using System.Net;
using System.Configuration;
using System.Data.SqlClient;
using System.Threading.Tasks;
using System.Text;
using Newtonsoft.Json;
public static async Task<HttpResponseMessage> Run(HttpRequestMessage req, TraceWriter log)
{
   log.Info("C# HTTP trigger function processed a request.");
var str = ConfigurationManager.ConnectionStrings["sqldb_connection"].ConnectionString;
using (SqlConnection conn = new SqlConnection(str))
   {
       conn.Open();
var text = "SELECT Top 100 Temperature, Moisture, UVLight, Humidity from dbo.SmartPlant Order by DateCreated DESC";
       SmartPlant ret = new SmartPlant();
using (SqlCommand cmd = new SqlCommand(text, conn))
       {
           SqlDataReader reader = await cmd.ExecuteReaderAsync();
try
           {
while (reader.Read())
               {
                   ret.Temperature = (int)reader[0];
                   ret.Moisture = (int)reader[1];
                   ret.UVLight = (int)reader[2];
                   ret.Humidity = (int)reader[3];
               }
           }
finally
           {
// Always call Close when done reading.
               reader.Close();
           }
var json = JsonConvert.SerializeObject(ret, Formatting.Indented);
return new HttpResponseMessage(HttpStatusCode.OK) 
           {
               Content = new StringContent(json, Encoding.UTF8, "application/json")
           };        
       }
   }
}
public class SmartPlant
{
    public float Temperature { get; set; }
    public float Moisture { get; set; }
    public float UVLight { get; set; }
    public float Humidity { get; set; }
}