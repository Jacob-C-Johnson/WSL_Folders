import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
from matplotlib.ticker import MaxNLocator

# Set plot style
plt.style.use('ggplot')
plt.rcParams['figure.figsize'] = (12, 8)
plt.rcParams['font.size'] = 12

# Create output directory for plots
os.makedirs('plots', exist_ok=True)

# Load data
df = pd.read_csv('computation_times.csv')

# Convert columns to appropriate types
df['n'] = df['n'].astype(int)
df['p'] = df['p'].astype(int)
df['t'] = df['t'].astype(int)
df['overall'] = df['overall'].astype(float)
df['computation'] = df['computation'].astype(float)
df['other'] = df['other'].astype(float)

# Get unique values
implementations = df['impl'].unique()
problem_sizes = sorted(df['n'].unique())
processor_counts = sorted(df['p'].unique())

# Define colors for consistency
colors = {'overall': 'blue', 'computation': 'green', 'other': 'red'}

# 1. T_overall, T_computation, T_other vs. p for each value of n
for impl in implementations:
    impl_data = df[df['impl'] == impl]

    for n in problem_sizes:
        n_data = impl_data[impl_data['n'] == n]

        if n_data.empty:
            continue

        plt.figure()
        plt.plot(n_data['p'], n_data['overall'], 'o-', color=colors['overall'], label='Overall Time')
        plt.plot(n_data['p'], n_data['computation'], 's-', color=colors['computation'], label='Computation Time')
        plt.plot(n_data['p'], n_data['other'], '^-', color=colors['other'], label='Other Time')

        plt.xscale('log', base=2)
        plt.yscale('log')
        plt.grid(True, which="both", ls="-", alpha=0.2)
        plt.xlabel('Number of Processors (p)')
        plt.ylabel('Time (seconds)')
        plt.title(f'Execution Time vs. Processors ({impl}, n={n})')
        plt.legend()
        plt.xticks(processor_counts)
        plt.gca().xaxis.set_major_formatter(plt.ScalarFormatter())
        plt.tight_layout()
        plt.savefig(f'plots/Execution_vs_Processors/time_vs_p_{impl}_n{n}.png', dpi=300)
        plt.close()

for impl in implementations:
    impl_data = df[df['impl'] == impl].copy()  # <---- add .copy() here!

    if impl_data.empty:
        continue

    fig, ax = plt.subplots()

    impl_data['label'] = impl_data.apply(lambda row: f"({row['p']},{row['n']})", axis=1)
    impl_data = impl_data.sort_values(['n', 'p'])

    bar_positions = np.arange(len(impl_data))
    bar_width = 0.8

    ax.bar(bar_positions, impl_data['computation'], bar_width,
           label='Computation', color=colors['computation'])
    ax.bar(bar_positions, impl_data['other'], bar_width,
           bottom=impl_data['computation'], label='Other', color=colors['other'])

    ax.set_xlabel('(Processors, Matrix Size)')
    ax.set_ylabel('Time (seconds)')
    ax.set_title(f'Computation vs. Overhead ({impl})')
    ax.set_xticks(bar_positions)
    ax.set_xticklabels(impl_data['label'], rotation=90)
    ax.legend()

    plt.tight_layout()
    os.makedirs('plots/Stacked_Bars', exist_ok=True)
    plt.savefig(f'plots/Stacked_Bars/stacked_bar_{impl}.png', dpi=300)
    plt.close()


# Function to calculate speedup and efficiency
def calculate_metrics(data, time_col):
    # Sort by processor count
    data = data.sort_values('p')

    # Get sequential time (p=1)
    seq_time = data[data['p'] == 1][time_col].values[0] if 1 in data['p'].values else data[time_col].min()

    # Calculate speedup and efficiency
    data['speedup'] = seq_time / data[time_col]
    data['efficiency'] = data['speedup'] / data['p']

    return data


# 3-6. Speedup and Efficiency plots
for impl in implementations:
    impl_data = df[df['impl'] == impl]

    if impl_data.empty or 1 not in impl_data['p'].values:
        continue

    # Overall speedup
    plt.figure()
    max_p = impl_data['p'].max()

    # Plot ideal speedup
    p_values = np.array(processor_counts)
    plt.plot(p_values, p_values, '--', color='black', label='Ideal')

    for n in problem_sizes:
        n_data = impl_data[impl_data['n'] == n]

        if n_data.empty:
            continue

        # Calculate speedup
        n_data = calculate_metrics(n_data, 'overall')

        plt.plot(n_data['p'], n_data['speedup'], 'o-', label=f'n={n}')

    plt.xscale('log', base=2)

    plt.grid(True, which="both", ls="-", alpha=0.2)
    plt.xlabel('Number of Processors (p)')
    plt.ylabel('Speedup')
    plt.title(f'Overall Speedup vs. Processors ({impl})')
    plt.legend()
    plt.xticks(processor_counts)
    plt.gca().xaxis.set_major_formatter(plt.ScalarFormatter())
    plt.tight_layout()
    plt.savefig(f'plots/Overall_Speedup_vs_Processors/overall_speedup_{impl}.png', dpi=300)
    plt.close()

    # Overall efficiency
    plt.figure()

    # Plot ideal efficiency
    plt.axhline(y=1, linestyle='--', color='black', label='Ideal')

    for n in problem_sizes:
        n_data = impl_data[impl_data['n'] == n]

        if n_data.empty:
            continue

        # Calculate efficiency (already done in speedup calculation)
        n_data = calculate_metrics(n_data, 'overall')

        plt.plot(n_data['p'], n_data['efficiency'], 'o-', label=f'n={n}')

    plt.xscale('log', base=2)
    plt.xlabel('Number of Processors (p)')
    plt.ylabel('Efficiency')
    plt.title(f'Overall Efficiency vs. Processors ({impl})')
    plt.legend()
    plt.xticks(processor_counts)
    plt.gca().xaxis.set_major_formatter(plt.ScalarFormatter())
    plt.ylim(0, 1.1)
    plt.tight_layout()
    plt.savefig(f'plots/Overall_Efficiency_vs_Processors/overall_efficiency_{impl}.png', dpi=300)
    plt.close()

    # Computation speedup
    plt.figure()

    # Plot ideal speedup
    plt.plot(p_values, p_values, '--', color='black', label='Ideal')

    for n in problem_sizes:
        n_data = impl_data[impl_data['n'] == n]

        if n_data.empty:
            continue

        # Calculate speedup for computation time
        n_data = calculate_metrics(n_data, 'computation')

        plt.plot(n_data['p'], n_data['speedup'], 'o-', label=f'n={n}')

    plt.xscale('log', base=2)

    plt.grid(True, which="both", ls="-", alpha=0.2)
    plt.xlabel('Number of Processors (p)')
    plt.ylabel('Speedup')
    plt.title(f'Computation Speedup vs. Processors ({impl})')
    plt.legend()
    plt.xticks(processor_counts)
    plt.gca().xaxis.set_major_formatter(plt.ScalarFormatter())
    plt.tight_layout()
    plt.savefig(f'plots/Computation_Speedup/computation_speedup_{impl}.png', dpi=300)
    plt.close()

    # Computation efficiency
    plt.figure()

    # Plot ideal efficiency
    plt.axhline(y=1, linestyle='--', color='black', label='Ideal')

    for n in problem_sizes:
        n_data = impl_data[impl_data['n'] == n]

        if n_data.empty:
            continue

        # Calculate efficiency for computation time
        n_data = calculate_metrics(n_data, 'computation')

        plt.plot(n_data['p'], n_data['efficiency'], 'o-', label=f'n={n}')

    plt.xscale('log', base=2)
    plt.xlabel('Number of Processors (p)')
    plt.ylabel('Efficiency')
    plt.title(f'Computation Efficiency vs. Processors ({impl})')
    plt.legend()
    plt.xticks(processor_counts)
    plt.gca().xaxis.set_major_formatter(plt.ScalarFormatter())
    plt.ylim(0, 1.1)
    plt.tight_layout()
    plt.savefig(f'plots/Computation_Efficiency/computation_efficiency_{impl}.png', dpi=300)
    plt.close()

print("All plots generated successfully in the 'plots' directory.")