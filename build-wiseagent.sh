#!/bin/bash

src_home=`pwd`
tmp_build_dir=${src_home}/build
release_dir=${src_home}/Release

precomnpile_3rdparty()
{
	cd ${src_home}/Library3rdParty/AdvLog
	autoreconf -i-f
	chmod 755 configure
	./configure
	make clean
	make
	make install DESTDIR=${tmp_build_dir}

	cd ${src_home}/Library3rdParty/curl-7.47.1
	autoreconf -i-f
	chmod 755 configure
	./configure
	make clean
	make
	make install DESTDIR=${tmp_build_dir}

	cd ${src_home}/Library3rdParty/libmodbus-3.1.2
	autoreconf -i-f
	chmod 755 configure
	./configure
	make clean
	make
	make install DESTDIR=${tmp_build_dir}
}

compile_agent()
{
	cd ${src_home}
	if [ ! -d "${src_home}/m4" ]; then
		mkdir "${src_home}/m4"
	fi

	autoreconf -i-f
	chmod 755 configure
	./configure
	make clean
	make
	make install DESTDIR=${tmp_build_dir}
}

generate_release()
{
	if [ ! -d "${release_dir}" ]; then
		mkdir "${release_dir}"
	fi

	if [ ! -d "${release_dir}/module" ]; then
		mkdir "${release_dir}/module"
	fi

	cp -f ${tmp_build_dir}/usr/local/lib/*.so* ${release_dir}/module/
	cp -f ${src_home}/Sample/HandlerSample/module_config.xml ${release_dir}/module/
	mv -f ${release_dir}/module/lib*.so* ${release_dir}/
	cp -f ${tmp_build_dir}/usr/local/bin/sampleagent ${release_dir}/
}

precomnpile_3rdparty
compile_agent
generate_release
exit 1 

