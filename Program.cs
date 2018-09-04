using System;
using System.IO;
using System.Linq;

namespace BFSAR_Split
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            if (args.Length != 1)
            {
                Console.WriteLine("\a\nUsage: BFSAR_Split.exe <Input.bfsar>");
            }
            else
            {
                var Strm = File.OpenRead(args[0]);
                var Reader = new BinaryReader(Strm);

                Directory.CreateDirectory($@"{args[0]}_Extracted");
                try
                {
                    while (true)
                    {
                        int TryRdMagic = Reader.ReadInt32();

                        void Extract(string Extension)
                        {
                            long PosOffs = Reader.ReadInt64();
                            int SizeOf = Reader.ReadInt32();
                            byte[] Data = Reader.ReadBytes(SizeOf - 0x10);

                            int RelPos = (int)Strm.Position - SizeOf;

                            string Range = $"0x{RelPos:x}-0x{RelPos + SizeOf:x}";

                            Console.WriteLine($"Extracting {Extension} at {Range}...");

                            var WrtStrm = File.OpenWrite($@"{args[0]}_Extracted/{Range}.{Extension}");
                            var Writer = new BinaryWriter(WrtStrm);

                            Writer.Write(TryRdMagic);
                            Writer.Write(PosOffs);
                            Writer.Write(SizeOf);
                            Writer.Write(Data);

                            Writer.Dispose();
                            WrtStrm.Dispose();

                            Strm.Position -= Strm.Position % 4;
                        }

                        if (TryRdMagic == 0x56415746)
                        {
                            Extract("bfwav");
                        }
                        else if (TryRdMagic == 0x50545346)
                        {
                            Extract("bfstp");
                        }
                        else if (TryRdMagic == 0x51455346)
                        {
                            Extract("bfseq");
                        }
                    }
                }
                catch (EndOfStreamException)
                {
                    Console.WriteLine("\nDone!");
                }
            }
        }
    }
}
