using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace SevenZap
{
	public static class SevenZap
	{
		public static void UpdateArchive(string archivePath, IEnumerable<string> files, CancellationToken? cancelToken = null)
		{
			List<FileEntry> entries = new List<FileEntry>(files.Select(f => new FileEntry() { FullPath = f }));
			UpdateArchive(archivePath, entries, cancelToken);
		}

		public static void UpdateArchive(string archivePath, IEnumerable<FileEntry> fileEntries, CancellationToken? cancelToken = null)
		{
			using (ManualResetEvent cancelEvent = new ManualResetEvent(false))
			{
				if (cancelToken.HasValue)
				{
					cancelToken.Value.Register(() => cancelEvent.Set());
				}

				List<string> files = new List<string>(fileEntries.Select(f => f.FullPath));
				List<string> entries = new List<string>(fileEntries.Select(f => f.EntryName));

				PInvoke.UpdateArchive(archivePath, cancelEvent, files, entries);
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
