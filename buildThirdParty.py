import os
import subprocess
import shutil
import platform

thirdBuildFolder = "tmpBuild"
thirdPartyPath = "third-party"
platform = platform.system()

if os.path.exists(thirdBuildFolder) == False:
	os.mkdir(thirdBuildFolder)

tbbSourcePath = os.path.join(thirdPartyPath, "oneTBB")
tbbInstallPath = os.path.join(thirdPartyPath, "tbb")
tbbBuildPath = os.path.join(thirdBuildFolder, "tbb")

if os.path.exists(tbbBuildPath) == False:
	os.mkdir(tbbBuildPath)

subprocess.run(["cmake", "-DCMAKE_INSTALL_PREFIX=" + tbbInstallPath, "-DTBB_TEST=OFF", "-DCMAKE_BUILD_TYPE=Release", "-S", tbbSourcePath, "-B", tbbBuildPath])
subprocess.run(["cmake", "--build", tbbBuildPath, "--config", "Release"])
subprocess.run(["cmake", "--install", tbbBuildPath])


oidnSourcePath = os.path.join(thirdPartyPath, "oidn")
oidnInstallPath = os.path.join(thirdPartyPath, "oidnlib")
oidnBuildPath = os.path.join(thirdBuildFolder, "oidn")

if os.path.exists(oidnBuildPath) == False:
	os.mkdir(oidnBuildPath)

if platform == 'Windows':
	subprocess.run(["cmake", "-DCMAKE_INSTALL_PREFIX=" + oidnInstallPath, "-DTBB_TEST=OFF", "-DCMAKE_BUILD_TYPE=Release", "-D TBB_ROOT="+tbbInstallPath, "-S", oidnSourcePath, "-B", oidnBuildPath])
elif platform == 'Darwin':
	subprocess.run(["cmake", "-DCMAKE_INSTALL_PREFIX=" + oidnInstallPath, "-DCMAKE_CXX_COMPILER=clang++", "-DCMAKE_C_COMPILER=clang", "-DTBB_TEST=OFF", "-DCMAKE_BUILD_TYPE=Release", "-D TBB_ROOT="+tbbInstallPath, "-S", oidnSourcePath, "-B", oidnBuildPath])

subprocess.run(["cmake", "--build", oidnBuildPath, "--config", "Release"])
subprocess.run(["cmake", "--install", ".", "--prefix", oidnBuildPath])


#shutil.rmtree(thirdBuildFolder, ignore_errors=True)


print("Build Third Party Process Done!!")