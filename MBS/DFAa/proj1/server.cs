using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.IO;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            string file = "";
            byte[] data = new byte[1024];
            IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 1488);
            UdpClient newsock = new UdpClient(ipep);
            IPEndPoint sender = new IPEndPoint(IPAddress.Any, 0);
            try
            {               
                data = newsock.Receive(ref sender);
                Console.WriteLine("Message received from {0}:", sender.ToString());
                file += Encoding.ASCII.GetString(data, 0, data.Length);
                while (true)
                {
                    data = newsock.Receive(ref sender);
                    newsock.Client.ReceiveTimeout = 2000;
                    file += Encoding.ASCII.GetString(data, 0, data.Length);
                }
                throw new Exception();
            } 
            catch(Exception e)
            {
                Console.WriteLine(file);
                string fullSavePath = "./sensitiveData.jpg";
                var bytes = Convert.FromBase64String(file);
                File.WriteAllBytes(fullSavePath, Convert.FromBase64String(file));
            }
        }
    }
}