# timelog

## About 

Little tool to add elapsed time in output for processes.

Highly optimized using C and respecting readline output (like from prompts with no endings). It does NOT simply replace linebreaks like with `sed /s/^/addprefix/g` BUT scans for `\n` in the output stream.


## Usage

```bash
Usage: timelog [-p prefix] [-s]
  -p, --prefix STRING      String to prefix before each linebreak.
  -s, --show-start-end     Show start and end timestamps with timezone.

```

### Example

```bash
$ (for i in {1..100}; do printf "rand-$i-%d\n" "$RANDOM"; done; bash -c 'read -p "Enter text: "' ) 2>&1 | timelog -p "${0}:[$$]" -s
```

```
/bin/zsh:[3313088][  0:00.00003m] Started at 2024-10-21 12:14:29 CEST
/bin/zsh:[3313088][  0:00.00005m] rand-1-19091
/bin/zsh:[3313088][  0:00.00005m] rand-2-15951
/bin/zsh:[3313088][  0:00.00005m] rand-3-3409
…
/bin/zsh:[3313088][  0:00.00014m] rand-98-6785
/bin/zsh:[3313088][  0:00.00014m] rand-99-20563
/bin/zsh:[3313088][  0:00.00014m] rand-100-15080
/bin/zsh:[3313088][  0:00.00066m] Enter text: ▉
```

## Benchmarks

I tried some other languages (since the tool should originally best just a simple script). These versions are not as optimized as the final C-version or share the correct logic.

```bash
$ hyperfine -m 100 'make test-output | ./incubate/awk/timelog-1'
Benchmark 1: make test-output | ./incubate/awk/timelog-1 
  Time (mean ± σ):       3.5 ms ±   0.8 ms    [User: 4.2 ms, System: 1.1 ms]
  Range (min … max):     2.8 ms …  17.6 ms    770 runs

$  hyperfine -m 100 'make test-output | ./incubate/perl/timelog-1'
Benchmark 1: make test-output | ./incubate/perl/timelog-1 
  Time (mean ± σ):      10.9 ms ±   1.5 ms    [User: 12.2 ms, System: 2.5 ms]
  Range (min … max):     9.0 ms …  24.0 ms    122 runs

$ hyperfine -m 100 'make test-output | ./incubate/python/timelog-1 '
Benchmark 1: make test-output | ./incubate/python/timelog-1 
  Time (mean ± σ):      13.9 ms ±   1.5 ms    [User: 14.0 ms, System: 3.2 ms]
  Range (min … max):    12.0 ms …  24.7 ms    119 runs

$ hyperfine -m 100 'make test-output | ./incubate/bash/timelog-1'
Benchmark 1: make test-output | ./incubate/bash/timelog-1 
  Time (mean ± σ):      36.8 ms ±   1.4 ms    [User: 31.0 ms, System: 7.4 ms]
  Range (min … max):    34.5 ms …  44.4 ms    100 runs

$ hyperfine -m 100 'make test-output | ./incubate/c/timelog-2'
Benchmark 1: make test-output | ./incubate/c/timelog-2
  Time (mean ± σ):       3.3 ms ±   0.3 ms    [User: 3.5 ms, System: 0.5 ms]
  Range (min … max):     2.8 ms …   6.4 ms    785 runs

$ hyperfine -m 100 'make test-output | ./dist/x86_64-linux-gnu-static/bin/timelog'
Benchmark 1: make test-output | ./dist/x86_64-linux-gnu-static/bin/timelog
  Time (mean ± σ):       3.1 ms ±   0.2 ms    [User: 3.3 ms, System: 0.4 ms]
  Range (min … max):     2.8 ms …   6.4 ms    747 runs

$ hyperfine -m 100 'make test-output | ./incubate/lua/timelog-1'
Benchmark 1: make test-output | ./incubate/lua/timelog-1
  Time (mean ± σ):       3.4 ms ±   0.3 ms    [User: 4.5 ms, System: 0.8 ms]
  Range (min … max):     2.8 ms …   5.6 ms    769 runs
```

Notes:

- AWK is not suitable due to line processing and not knowing C-like fread or fgets
- Due BASH is also mostly line-based it can be used and can also be pretty fast only using built-ins
- I didn't expect LUA to be so fast. If that script works on every platform – needs to be tested – great!
- Python of course can also be optimized and is also good for beeing used on any platform


Any of those versions would be suitable for processing a lot of log output.