
@excludes = qw(
 registry.cpp
 colorer-doc.cpp
 compress.c
 deflate.c
 example.c
 gzio.c
 maketree.c
 minigzip.c
 trees.c
 iowin32.c
 minizip.c
 zip.c
);

sub traverse
{
    my $dir = shift;
    my @list = `ls $dir`;

    $dir =~ /\\([^\\]+)$/;
    print F "<Filter Name=\"$1\">\n";

    foreach $entry (@list){
        chomp $entry;
        
        if (-d $dir.'/'.$entry) {
            traverse($dir.'\\'.$entry);
        }

        next unless ($entry =~ /^(.*)\.(c|cpp|h)$/);
        next if grep(/$entry/, @excludes);
        if (-f $dir."/".$entry)
        {
           print F "<File RelativePath=\"..\\shared\\$dir\\$entry\">\n</File>\n";
        }
    }
    print F "</Filter>\n";
}



open F, ">colorer.vcproj";

print F <<INPUT;
<?xml version="1.0" encoding="windows-1251"?>
<VisualStudioProject
	ProjectType="Visual C++"
	Version="7.10"
	Name="colorer"
	ProjectGUID="{1D34D8DB-0A9A-46B8-83BD-07D38B989EF2}"
	Keyword="Win32Proj">
	<Platforms>
		<Platform
			Name="Win32"/>
	</Platforms>
	<Configurations>
		<Configuration
			Name="Debug|Win32"
			OutputDirectory="Debug"
			IntermediateDirectory="Debug"
			ConfigurationType="1"
			CharacterSet="2">
			<Tool
				Name="VCCLCompilerTool"
				AdditionalOptions=""
				Optimization="0"
				AdditionalIncludeDirectories="../shared;../zlib"
				PreprocessorDefinitions="WIN32;_DEBUG;_CONSOLE;COLORER_FEATURE_USE_DL_MALLOC=FALSE;COLORER_FEATURE_USE_CHUNK_ALLOC=FALSE;_CRT_SECURE_NO_DEPRECATE"
				MinimalRebuild="TRUE"
				BasicRuntimeChecks="3"
				RuntimeLibrary="1"
				UsePrecompiledHeader="0"
				WarningLevel="3"
				Detect64BitPortabilityProblems="TRUE"
				DebugInformationFormat="4"
				DisableSpecificWarnings="4308,4307"/>
			<Tool
				Name="VCCustomBuildTool"/>
			<Tool
				Name="VCLinkerTool"
				AdditionalOptions=""
				AdditionalDependencies="wininet.lib"
				OutputFile="\$(OutDir)/colorer.exe"
				LinkIncremental="2"
				SuppressStartupBanner="TRUE"
				GenerateDebugInformation="TRUE"
				ProgramDatabaseFile="\$(OutDir)/colorer.pdb"
				SubSystem="1"
				TargetMachine="1"/>
			<Tool
				Name="VCMIDLTool"/>
			<Tool
				Name="VCPostBuildEventTool"/>
			<Tool
				Name="VCPreBuildEventTool"/>
			<Tool
				Name="VCPreLinkEventTool"/>
			<Tool
				Name="VCResourceCompilerTool"/>
			<Tool
				Name="VCWebServiceProxyGeneratorTool"/>
			<Tool
				Name="VCXMLDataGeneratorTool"/>
			<Tool
				Name="VCWebDeploymentTool"/>
			<Tool
				Name="VCManagedWrapperGeneratorTool"/>
			<Tool
				Name="VCAuxiliaryManagedWrapperGeneratorTool"/>
		</Configuration>
		<Configuration
			Name="Release|Win32"
			OutputDirectory="Release"
			IntermediateDirectory="Release"
			ConfigurationType="1"
			CharacterSet="2">
			<Tool
				Name="VCCLCompilerTool"
				AdditionalOptions=""
				Optimization="3"
				InlineFunctionExpansion="2"
				FavorSizeOrSpeed="1"
				AdditionalIncludeDirectories="../shared;../zlib"
				PreprocessorDefinitions="WIN32;NDEBUG;_CONSOLE;_CRT_SECURE_NO_DEPRECATE"
				RuntimeLibrary="0"
				UsePrecompiledHeader="0"
				WarningLevel="3"
				Detect64BitPortabilityProblems="TRUE"
				DebugInformationFormat="3"
				DisableSpecificWarnings="4308,4307"/>
			<Tool
				Name="VCCustomBuildTool"/>
			<Tool
				Name="VCLinkerTool"
				AdditionalOptions=""
				AdditionalDependencies="wininet.lib"
				OutputFile="\$(OutDir)/colorer.exe"
				LinkIncremental="1"
				SuppressStartupBanner="TRUE"
				GenerateDebugInformation="TRUE"
				SubSystem="1"
				OptimizeReferences="2"
				EnableCOMDATFolding="2"
				TargetMachine="1"/>
			<Tool
				Name="VCMIDLTool"/>
			<Tool
				Name="VCPostBuildEventTool"/>
			<Tool
				Name="VCPreBuildEventTool"/>
			<Tool
				Name="VCPreLinkEventTool"/>
			<Tool
				Name="VCResourceCompilerTool"/>
			<Tool
				Name="VCWebServiceProxyGeneratorTool"/>
			<Tool
				Name="VCXMLDataGeneratorTool"/>
			<Tool
				Name="VCWebDeploymentTool"/>
			<Tool
				Name="VCManagedWrapperGeneratorTool"/>
			<Tool
				Name="VCAuxiliaryManagedWrapperGeneratorTool"/>
		</Configuration>
	</Configurations>
	<References>
	</References>
	<Files>
INPUT

traverse('..\\shared');
traverse('..\\zlib');

print F <<INPUT;
	</Files>
	<Globals>
	</Globals>
</VisualStudioProject>
INPUT

close F;

