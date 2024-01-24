using System;
using System.Collections.Generic;
using System.IO;

namespace SevenZap
{
	internal static class Program
	{
		public static int Main(params string[] args)
		{
			if (args.Length < 2)
			{
				Console.WriteLine("Expecting command: <archive.7z> <filePath> [<filePath2> [<filePath3>] ...]");
				return 1;
			}

			try
			{
				string archive = args[0];
				List<string> files = new List<string>(args);
				files.RemoveAt(0);
				SevenZap.UpdateArchive(archive, files);

				string targetFolder = Path.Combine(Path.GetDirectoryName(archive), Path.GetFileNameWithoutExtension(archive));
				SevenZap.Extract(archive, targetFolder);
			}
			catch (Exception ex)
			{
				Console.WriteLine($"Failed to update archive. {ex.Message}");
				while (ex.InnerException != null)
				{
					ex = ex.InnerException;
					Console.WriteLine($"\t. {ex.Message}");
				}
				return 1;
			}

			return 0;
		}
	}
}
