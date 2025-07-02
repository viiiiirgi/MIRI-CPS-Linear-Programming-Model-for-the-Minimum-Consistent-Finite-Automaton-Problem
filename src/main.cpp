#include <ilcplex/ilocplex.h>
#include <vector>
#include <string>
#include <sstream>

ILOSTLBEGIN

const int MAX_N_STATES = 10;

vector<vector<int>> sequences_bits;
vector<bool> sequences_should_accepting_varsept;
string input_copy_for_print;
int max_sequence_len;
int total_sequences_count;

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    stringstream input_buffer_ss_for_echo;
    string line_str;

    total_sequences_count = 0;
    max_sequence_len = 0;

    int num_accepting_varsept_seqs;
    if (!getline(cin, line_str)) return 1;
    input_buffer_ss_for_echo << line_str << "\n";
    stringstream ss_num_accepting_varsept(line_str);
    if (!(ss_num_accepting_varsept >> num_accepting_varsept_seqs)) return 1;

    for (int i = 0; i < num_accepting_varsept_seqs; ++i) {
        if (!getline(cin, line_str)) return 1;
        input_buffer_ss_for_echo << line_str << "\n";
        stringstream ss_seq_parser(line_str);
        int len;
        if (!(ss_seq_parser >> len)) return 1;
        if (len > max_sequence_len) max_sequence_len = len;
        vector<int> bits_vec(len);
        for (int j = 0; j < len; ++j) {
            if (!(ss_seq_parser >> bits_vec[j])) return 1;
        }
        sequences_bits.push_back(bits_vec);
        sequences_should_accepting_varsept.push_back(true);
        total_sequences_count++;
    }

    if (!getline(cin, line_str)) return 1;
    input_buffer_ss_for_echo << line_str << "\n";

    string reject_count_line = line_str;
    if (line_str.empty()) {
        if (!getline(cin, reject_count_line)) return 1;
        input_buffer_ss_for_echo << reject_count_line << "\n";
    }

    int num_reject_seqs;
    stringstream ss_num_reject(reject_count_line);
    if (!(ss_num_reject >> num_reject_seqs)) return 1;

    for (int i = 0; i < num_reject_seqs; ++i) {
        if (!getline(cin, line_str)) return 1;
        input_buffer_ss_for_echo << line_str << "\n";
        stringstream ss_seq_parser(line_str);
        int len;
        if (!(ss_seq_parser >> len)) return 1;
        if (len > max_sequence_len) max_sequence_len = len;
        vector<int> bits_vec(len);
        for (int j = 0; j < len; ++j) {
            if (!(ss_seq_parser >> bits_vec[j])) return 1;
        }
        sequences_bits.push_back(bits_vec);
        sequences_should_accepting_varsept.push_back(false);
        total_sequences_count++;
    }
    input_copy_for_print = input_buffer_ss_for_echo.str();
    if (!input_copy_for_print.empty() && input_copy_for_print.back() == '\n') {
        input_copy_for_print.pop_back();
    }

    for (int N_current_states = 1; N_current_states <= MAX_N_STATES; ++N_current_states) {
        IloEnv env;
        IloModel model(env);

        IloArray<IloArray<IloBoolVarArray>> transition_vars(env, N_current_states);
        for (int i = 0; i < N_current_states; ++i) {
            transition_vars[i] = IloArray<IloBoolVarArray>(env, 2);
            for (int b = 0; b < 2; ++b) {
                transition_vars[i][b] = IloBoolVarArray(env, N_current_states);
            }
        }

        IloBoolVarArray accepting_vars(env, N_current_states);

        IloArray<IloArray<IloBoolVarArray>> run_state_vars(env, total_sequences_count);
        for (int p = 0; p < total_sequences_count; ++p) {
            int current_seq_len = sequences_bits[p].size();
            run_state_vars[p] = IloArray<IloBoolVarArray>(env, current_seq_len + 1);
            for (int k = 0; k <= current_seq_len; ++k) {
                run_state_vars[p][k] = IloBoolVarArray(env, N_current_states);
            }
        }

        IloArray<IloBoolVarArray> run_accept_vars(env, total_sequences_count);
        for (int p = 0; p < total_sequences_count; ++p) {
            run_accept_vars[p] = IloBoolVarArray(env, N_current_states);
        }

        for (int i = 0; i < N_current_states; ++i) {
            for (int b = 0; b < 2; ++b) {
                IloExpr sum_expr(env);
                for (int j = 0; j < N_current_states; ++j) {
                    sum_expr += transition_vars[i][b][j];
                }
                model.add(sum_expr == 1);
                sum_expr.end();
            }
        }

        for (int p = 0; p < total_sequences_count; ++p) {
            int current_seq_len = sequences_bits[p].size();

            model.add(run_state_vars[p][0][0] == 1);
            for (int st = 1; st < N_current_states; ++st) {
                model.add(run_state_vars[p][0][st] == 0);
            }

            for (int k = 0; k <= current_seq_len; ++k) {
                IloExpr sum_expr(env);
                for (int st = 0; st < N_current_states; ++st) {
                    sum_expr += run_state_vars[p][k][st];
                }
                model.add(sum_expr == 1);
                sum_expr.end();
            }

            for (int k_step = 0; k_step < current_seq_len; ++k_step) {
                int bit_val = sequences_bits[p][k_step];
                for (int i_curr = 0; i_curr < N_current_states; ++i_curr) {
                    for (int j_next = 0; j_next < N_current_states; ++j_next) {
                        model.add(run_state_vars[p][k_step + 1][j_next] - run_state_vars[p][k_step][i_curr] - transition_vars[i_curr][bit_val][j_next] >= -1);
                    }
                }
            }

            for (int st = 0; st < N_current_states; ++st) {
                model.add(run_accept_vars[p][st] - run_state_vars[p][current_seq_len][st] <= 0);
                model.add(run_accept_vars[p][st] - accepting_vars[st] <= 0);
                model.add(run_accept_vars[p][st] - run_state_vars[p][current_seq_len][st] - accepting_vars[st] >= -1);
            }

            IloExpr final_sum_expr(env);
            for (int st = 0; st < N_current_states; ++st) {
                final_sum_expr += run_accept_vars[p][st];
            }

            if (sequences_should_accepting_varsept[p]) {
                model.add(final_sum_expr == 1);
            } else {
                model.add(final_sum_expr == 0);
            }
            final_sum_expr.end();
        }

        model.add(IloMinimize(env, 0.0));

        IloCplex cplex(model);
        cplex.setOut(env.getNullStream());
        cplex.setWarning(env.getNullStream());

        cerr << "DEBUG: Trying N = " << N_current_states << "..." << endl;
        if (cplex.solve() == IloTrue) {
            cout << input_copy_for_print << endl;
            cout << "" << endl;
            cout << N_current_states << endl;

            for (int i = 0; i < N_current_states; ++i) {
                int trans_0 = -1, trans_1 = -1;
                for (int j = 0; j < N_current_states; ++j) {
                    if (IloRound(cplex.getValue(transition_vars[i][0][j])) == 1) trans_0 = j;
                    if (IloRound(cplex.getValue(transition_vars[i][1][j])) == 1) trans_1 = j;
                }
                cout << trans_0 << " "
                     << trans_1 << " "
                     << (IloRound(cplex.getValue(accepting_vars[i])) == 1 ? 1 : 0) << endl;
            }
            env.end();
            return 0;
        }
        cerr << "DEBUG: No solution for N = " << N_current_states << endl;
        env.end();
    }

    cerr << "No solution found within N <= " << MAX_N_STATES << " states." << endl;
    return 1;
}