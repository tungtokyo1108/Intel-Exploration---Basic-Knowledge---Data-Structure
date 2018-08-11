/*
 * cppcompiler.h
 *
 *  Created on: Jul 19, 2018
 *      Student (Coder): Tung Dang
 */

#ifndef CPPCOMPILER_H_
#define CPPCOMPILER_H_

#ifndef _MSC_VER
#define ALIGN(size) _declspec(align(size))
#endif

#include <UFO/portable_compiler.h>
#include <UFO/fake_seh.h>

#define ALGN(size) _attribute_((aligned(size)))

#ifndef NO_EXCEPTION_HANDLING
#define try if (true)
#define catch(x) if(false)
#endif

#endif /* CPPCOMPILER_H_ */
