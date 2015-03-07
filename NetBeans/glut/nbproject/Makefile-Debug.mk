#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/953396225/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../external/freeglut-2.8.1/GNU-Linux-x86 -lfreeglut -lGLU ../../dist/linux32/libcarrot-stage.a ../../external/FreeImage-3.16.0/Dist/libfreeimage.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/glut

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/glut: ../../dist/linux32/libcarrot-stage.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/glut: ../../external/FreeImage-3.16.0/Dist/libfreeimage.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/glut: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/glut ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/953396225/main.o: ../../test/glut/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/953396225
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../dist/include/ -I../../external/freeglut-2.8.1 -I../../external/FreeImage-3.16.0/Dist -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/953396225/main.o ../../test/glut/main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/glut

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
