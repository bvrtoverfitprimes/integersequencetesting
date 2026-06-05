# Integer Sequences: Conjecture Testing

Testing a conjecture about composite numbers with special properties related to Euler's totient function and the divisor sum function.

## The Conjecture

The conjecture states:

$$\\{ \text{composite } n \mid \varphi(n+12) = \varphi(n) + 12 \land \sigma(n+12) = \sigma(n) + 12 \\} \implies n \equiv 65 \pmod{72}$$

In other words, for any composite number $n$ that satisfies **both** of the following conditions:
- The Euler totient function: $\varphi(n+12) = \varphi(n) + 12$
- The divisor sum function: $\sigma(n+12) = \sigma(n) + 12$

Then $n$ must be congruent to 65 modulo 72.

## Implementation

The code implements:

1. **Linear sieve** (`build_spf`) - Efficiently computes smallest prime factors up to a limit using the linear sieve algorithm in $O(n)$ time.

2. **Prime factorization** (`factorize`) - Uses the sieve to quickly factor integers.

3. **Totient and divisor sum** (`phi_sigma`) - Computes $\varphi(n)$ and $\sigma(n)$ using prime factorization formulas:
   - $\varphi(n) = \prod p_i^{e_i-1}(p_i - 1)$
   - $\sigma(n) = \prod \frac{p_i^{e_i+1} - 1}{p_i - 1}$

4. **Prime quadruplet detection** (`is_prime_quadruplet`) - Verifies if $(p, p+2, p+6, p+8)$ are all prime.

## Running the Tests

```bash
python testing.py
```

This will:
- Compute the sieve and totient/divisor sum values
- Search for all composite numbers satisfying both jump conditions
- Print a table of all solutions found
- Verify the conjecture properties
- Automatically save results to `test_results.json`

## Results

Testing so far has been performed up to $$10^10$$.

The test results are automatically saved to `test_results.json` containing:
- Total number of solutions found
- All solution values with their properties
- Verification summary

Note: Larger limits require more computation time and memory.

## Mathematical References

- **Euler's totient function** $\varphi(n)$: counts integers up to $n$ that are coprime to $n$
- **Divisor sum function** $\sigma(n)$: sum of all positive divisors of $n$
- **Prime quadruplet**: four primes of the form $(p, p+2, p+6, p+8)$
