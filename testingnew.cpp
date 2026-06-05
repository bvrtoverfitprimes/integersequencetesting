#include <bits/stdc++.h>
using namespace std;

static const int LIMIT = 100000000;

int main() {
    int N = LIMIT + 13;
    vector<int> spf(N, 0);
    vector<int> primes;

    for (int i = 2; i < N; i++) {
        if (spf[i] == 0) {
            spf[i] = i;
            primes.push_back(i);
        }
        for (int p : primes) {
            if (p > spf[i] || (long long)i * p >= N) break;
            spf[i * p] = p;
        }
    }

    auto phi_sigma = [&](int n) -> pair<long long, long long> {
        if (n == 1) return {1, 1};
        long long phi = 1, sig = 1;
        while (n > 1) {
            int p = spf[n], e = 0;
            while (n % p == 0) { n /= p; e++; }
            long long pe = 1;
            for (int i = 0; i < e; i++) pe *= p;
            phi *= (p - 1) * (pe / p);
            long long num = pe * p - 1;
            long long den = p - 1;
            sig *= num / den;
        }
        return {phi, sig};
    };

    vector<long long> phis(N), sigs(N);
    int last_pct_sieve = -1;
    for (int k = 1; k < N; k++) {
        auto [ph, sg] = phi_sigma(k);
        phis[k] = ph;
        sigs[k] = sg;
        int pct = (int)((long long)k * 100 / N);
        if (pct / 5 > last_pct_sieve / 5) {
            last_pct_sieve = pct;
            printf("Progress: %d%%\n", pct);
            fflush(stdout);
        }
    }

    vector<int> solutions;
    int last_pct_search = -1;
    for (int n = 2; n < LIMIT; n++) {
        int pct = (int)((long long)n * 100 / LIMIT);
        if (pct / 5 > last_pct_search / 5) {
            last_pct_search = pct;
            printf("Progress: %d%%\n", pct);
            fflush(stdout);
        }
        if (spf[n] == n) continue;
        if (phis[n + 12] - phis[n] != 12) continue;
        if (sigs[n + 12] - sigs[n] != 12) continue;
        solutions.push_back(n);
    }

    printf("Total solutions in [2, %d): %d\n", LIMIT, (int)solutions.size());

    set<int> residues;
    for (int n : solutions) residues.insert(n % 72);
    printf("Distinct residues mod 72: {");
    bool first = true;
    for (int r : residues) { if (!first) printf(", "); printf("%d", r); first = false; }
    printf("}\n\n");

    printf("Table 1 -- all composite n with both jump conditions:\n\n");
    printf("%6s %14s %6s %6s %20s %10s\n", "p", "n = p(p+8)", "p+2", "p+6", "n+12 = (p+2)(p+6)", "n mod 72");
    printf("%s\n", string(70, '-').c_str());

    bool all_from_quadruplet = true;
    bool all_residue_65 = true;

    for (int n : solutions) {
        int tmp = n;
        map<int,int> fact;
        while (tmp > 1) {
            int p = spf[tmp];
            while (tmp % p == 0) { fact[p]++; tmp /= p; }
        }
        vector<int> primes_n;
        for (auto& [p, e] : fact)
            for (int i = 0; i < e; i++) primes_n.push_back(p);

        if (primes_n.size() != 2 || primes_n[1] - primes_n[0] != 8) {
            printf("  ANOMALY: n = %d has unexpected factorization\n", n);
            all_from_quadruplet = false;
            continue;
        }
        int p = primes_n[0], q = primes_n[1];

        bool quad = (spf[p] == p && spf[p+2] == p+2 && spf[p+6] == p+6 && spf[p+8] == p+8);
        if (!quad) {
            printf("  ANOMALY: n = %d = %d * %d but not a prime quadruplet\n", n, p, q);
            all_from_quadruplet = false;
            continue;
        }

        if (n + 12 != (p + 2) * (p + 6)) {
            printf("  IDENTITY FAILED: %d + 12 != (%d+2)*(%d+6)\n", n, p, p);
            continue;
        }

        int r = n % 72;
        if (r != 65) all_residue_65 = false;
        printf("%6d %14d %6d %6d %20d %10d\n", p, n, p+2, p+6, n+12, r);
    }

    printf("\n  All solutions arise from prime quadruplets: %s\n", all_from_quadruplet ? "True" : "False");
    printf("  All solutions are 65 (mod 72):              %s\n", all_residue_65 ? "True" : "False");
    if (all_from_quadruplet && all_residue_65) printf("\nCONSISTENT.\n");

    FILE* jf = fopen("test_results.json", "w");
    fprintf(jf, "{\n  \"test_results\": {\n");
    fprintf(jf, "    \"limit\": %d,\n", LIMIT);
    fprintf(jf, "    \"total_solutions\": %d,\n", (int)solutions.size());
    fprintf(jf, "    \"distinct_residues_mod_72\": [");
    {
        bool f = true;
        for (int r : residues) { if (!f) fprintf(jf, ", "); fprintf(jf, "%d", r); f = false; }
    }
    fprintf(jf, "],\n");
    fprintf(jf, "    \"solutions\": [\n");
    for (int i = 0; i < (int)solutions.size(); i++) {
        int n = solutions[i];
        int tmp = n;
        map<int,int> fact2;
        while (tmp > 1) { int pp = spf[tmp]; while (tmp % pp == 0) { fact2[pp]++; tmp /= pp; } }
        vector<int> pv;
        for (auto& [pp, e] : fact2) for (int j = 0; j < e; j++) pv.push_back(pp);
        int p = pv[0];
        fprintf(jf, "      {\"p\": %d, \"n\": %d, \"p_plus_2\": %d, \"p_plus_6\": %d, \"n_plus_12\": %d, \"n_mod_72\": %d}%s\n",
            p, n, p+2, p+6, n+12, n%72, i+1 < (int)solutions.size() ? "," : "");
    }
    fprintf(jf, "    ],\n");
    fprintf(jf, "    \"verification\": {\n");
    fprintf(jf, "      \"all_from_quadruplet\": %s,\n", all_from_quadruplet ? "true" : "false");
    fprintf(jf, "      \"all_residue_65\": %s,\n", all_residue_65 ? "true" : "false");
    fprintf(jf, "      \"consistent_with_theorems\": %s\n", (all_from_quadruplet && all_residue_65) ? "true" : "false");
    fprintf(jf, "    }\n  }\n}\n");
    fclose(jf);

    return 0;
}
