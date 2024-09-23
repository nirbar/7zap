using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

namespace SevenZap
{
	public static class SevenZap
	{
		public enum CompressionLevel
		{
			X1_Fastest = 1,
			X2 = 2,
			X3 = 3,
			X4 = 4,
			X5_Medium = 5,
			X6 = 6,
			X7 = 7,
			X8 = 8,
			X9_Smallest = 9,
		};

		public static void UpdateArchive(string archivePath, IEnumerable<string> files, CancellationToken? cancelToken = null, CompressionLevel level = CompressionLevel.X5_Medium)
		{
			List<FileEntry> entries = new List<FileEntry>(files.Select(f => new FileEntry() { FullPath = f }));
			UpdateArchive(archivePath, entries, cancelToken, level);
		}

		public static void UpdateArchive(string archivePath, IEnumerable<FileEntry> fileEntries, CancellationToken? cancelToken = null, CompressionLevel level = CompressionLevel.X5_Medium)
		{
			using (ManualResetEvent cancelEvent = new ManualResetEvent(false))
			{
				if (cancelToken.HasValue)
				{
					cancelToken.Value.Register(() => cancelEvent.Set());
				}

				List<string> files = new List<string>(fileEntries.Select(f => Path.GetFullPath(f.FullPath)));
				List<string> entries = new List<string>(fileEntries.Select(f => f.EntryName));

				PInvoke.UpdateArchive(archivePath, cancelEvent, files, entries, level);
			}
		}

		public static void Extract(string archivePath, string targetFolder)
		{
			PInvoke.Extract7z(archivePath, targetFolder);
		}

		public struct FileEntry
		{
			public string FullPath { get; set; }
			public string EntryName { get; set; }
		}
	}
}
