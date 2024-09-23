using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;

namespace SevenZap
{
	internal static class PInvoke
	{
		#region Update7z
		[DllImport("win-x64\\7Zap", CallingConvention = CallingConvention.StdCall, PreserveSig = false, EntryPoint = "Update7z", CharSet = CharSet.Unicode)]
		private static extern void Update7z_x64(
			string archivePath, SafeWaitHandle hCancelEvent, int numFiles
			, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr, SizeParamIndex = 1)] string[] files
			, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr, SizeParamIndex = 1)] string[] entryNames
			, [MarshalAs(UnmanagedType.I4)] SevenZap.CompressionLevel level);

		[DllImport("win-x86\\7Zap", CallingConvention = CallingConvention.StdCall, PreserveSig = false, EntryPoint = "Update7z", CharSet = CharSet.Unicode)]
		private static extern void Update7z_x86(
			string archivePath, SafeWaitHandle hCancelEvent, int numFiles
			, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr, SizeParamIndex = 1)] string[] files
			, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr, SizeParamIndex = 1)] string[] entryNames
			, [MarshalAs(UnmanagedType.I4)] SevenZap.CompressionLevel level);

		[DllImport("win-arm64\\7Zap", CallingConvention = CallingConvention.StdCall, PreserveSig = false, EntryPoint = "Update7z", CharSet = CharSet.Unicode)]
		private static extern void Update7z_arm64(
			string archivePath, SafeWaitHandle hCancelEvent, int numFiles
			, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr, SizeParamIndex = 1)] string[] files
			, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr, SizeParamIndex = 1)] string[] entryNames
			, [MarshalAs(UnmanagedType.I4)] SevenZap.CompressionLevel level);

		internal static void UpdateArchive(string archivePath, ManualResetEvent cancelEvent, IEnumerable<string> files, IEnumerable<string> entryNames = null, SevenZap.CompressionLevel level = SevenZap.CompressionLevel.X5_Medium)
		{
			if (Environment.OSVersion.Platform != PlatformID.Win32NT)
			{
				throw new PlatformNotSupportedException();
			}

			if (RuntimeInformation.ProcessArchitecture == Architecture.Arm64)
			{
				Update7z_arm64(archivePath, cancelEvent.SafeWaitHandle, files.Count(), files.ToArray(), entryNames?.ToArray(), level);
				return;
			}

			if (Environment.Is64BitProcess)
			{
				Update7z_x64(archivePath, cancelEvent.SafeWaitHandle, files.Count(), files.ToArray(), entryNames?.ToArray(), level);
				return;
			}

			Update7z_x86(archivePath, cancelEvent.SafeWaitHandle, files.Count(), files.ToArray(), entryNames?.ToArray(), level);
			return;
		}
		#endregion

		#region Extract7z
		[DllImport("win-x64\\7Zap", CallingConvention = CallingConvention.StdCall, PreserveSig = false, EntryPoint = "Extract7z", CharSet = CharSet.Unicode)]
		private static extern void Extract7z_x64(string archivePath, string targetFolder);

		[DllImport("win-x86\\7Zap", CallingConvention = CallingConvention.StdCall, PreserveSig = false, EntryPoint = "Extract7z", CharSet = CharSet.Unicode)]
		private static extern void Extract7z_x86(string archivePath, string targetFolder);

		[DllImport("win-arm64\\7Zap", CallingConvention = CallingConvention.StdCall, PreserveSig = false, EntryPoint = "Extract7z", CharSet = CharSet.Unicode)]
		private static extern void Extract7z_arm64(string archivePath, string targetFolder);

		internal static void Extract7z(string archivePath, string targetFolder)
		{
			if (Environment.OSVersion.Platform != PlatformID.Win32NT)
			{
				throw new PlatformNotSupportedException();
			}

			if (RuntimeInformation.ProcessArchitecture == Architecture.Arm64)
			{
				Extract7z_arm64(archivePath, targetFolder);
				return;
			}

			if (Environment.Is64BitProcess)
			{
				Extract7z_x64(archivePath, targetFolder);
				return;
			}

			Extract7z_x86(archivePath, targetFolder);
			return;
		}
		#endregion
	}
}
