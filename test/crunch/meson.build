libCrunchTests = ['testArgsParser', 'testCrunch', 'testBad']
if not c11Threading
	libCrunchTests += 'testThreadShim'
endif

libCrunchPath = meson.global_build_root() / libCrunch.outdir()

foreach test : libCrunchTests
	command = [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + crunchSrcDir, '-L' + libCrunchPath]
	if isMSVC and coverage
		command = [coverageTarget, coverageRunner] + coverageArgs + [
			'cobertura:crunchMake-crunch-@0@-coverage.xml'.format(test), '--'
		] + command
	endif

	inputs = [files(test + '.c')]
	if test == 'testCrunch'
		command += ['-I' + ranluxSrcDir, '-lstdc++']
		inputs += [ranlux]
	elif test == 'testThreadShim'
		inputs += libCrunch.extract_objects('threading/threadShim.c')
	endif

	custom_target(
		'crunch-' + test,
		command: command + crunchSanitizer + commandExtra,
		input: inputs,
		output: test + testExt,
		depends: libCrunch,
		build_by_default: true
	)
endforeach

if not isMSVC or not coverage
test(
	'crunch',
	crunch,
	args: libCrunchTests + ['testMustNotExist'],
	workdir: meson.current_build_dir()
)

test(
	'crunch-empty',
	crunch,
	workdir: meson.current_build_dir(),
	should_fail: true
)
else
test(
	'crunch',
	coverageRunner,
	args: coverageArgs + [
		'cobertura:crunch-coverage.xml', '--', crunch
	] + libCrunchTests + ['testMustNotExist'],
	workdir: meson.current_build_dir()
)

test(
	'crunch-empty',
	coverageRunner,
	args: coverageArgs + ['cobertura:crunch-empty-coverage.xml', '--', crunch],
	workdir: meson.current_build_dir(),
	should_fail: true
)
endif
