import sys

with open(sys.argv[1], "r") as f:
    lines = f.readlines()

print("Checking number of lines in output...", end='')
if len(lines) != 11:
    print(f"\nExpecting 11 lines of output, got {len(lines)}")
    sys.exit(1)
print("ok")

iterations = []
residuals = []
print("Parsing output...", end='')
for i, line in enumerate(lines):
    try:
        it, rro0, eq, residual = line.strip().split(" ")
    except ValueError:
        print(f"\nUnexpected line format on line {i+1}: '{line.strip()}'")
        sys.exit(1)
    try:
        iterations.append(int(it))
    except ValueError:
        print(f"\nExpecting integer iteration counter on line {i+1}, found {it}")
        sys.exit(1)
    if rro0 != "||r||/||r0||":
        print(f"\nUnexpected token on line {i+1}, found '{rro0}', expected '||r||/||r0||'")
        sys.exit(1)
    if eq != "=":
        print(f"\nUnexpected token on line {i+1}, found '{eq}', expected '='")
        sys.exit(1)
    try:
        residuals.append(float(residual))
    except ValueError:
        print(f"\nExpecting floating point residual on line {i+1}, found {residual}")
        sys.exit(1)
print("ok")

expect = [0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000]
print("Checking iteration counts are correct...", end='')
if (iterations != expect):
    print("\nUnexpected iteration counts")
    print(f"Got {iterations}")
    print(f"Expected {expect}")
    sys.exit(1)
print("ok")

print("Checking first residual is correctv...", end='')
if residuals[0] != 1:
    print(f"\nExpecting first residual to be 1, got {residuals[0]}")
    sys.exit(1)
print("ok")

print("Checking residuals decrease monotonically...", end='')
if (residuals != sorted(residuals, reverse=True)):
    print("\nExpecting monotonically decreasing residuals")
    got = "\n".join(map(str, residuals))
    print(f"Got: \n{got}")
    sys.exit(1)
print("ok")

print("Checking final residual is small enough...", end='')
if residuals[-1] > 0.001:
    print(f"\nExpecting final residual <= 0.001, got {residuals[-1]}")
    sys.exit(1)
print("ok")
