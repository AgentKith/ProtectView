package ui

import (
	"testing"
)

func absFloat(x float64) float64 {
	if x < 0 {
		return -x
	}
	return x
}

func TestCalculateGrid(t *testing.T) {
	tests := []struct {
		name     string
		count    int
		wantRows int
		wantCols int
	}{
		{"zero cameras", 0, 1, 1},
		{"one camera", 1, 1, 1},
		{"two cameras", 2, 1, 2},
		{"four cameras", 4, 2, 2},
		{"nine cameras", 9, 3, 3},
		{"six cameras", 6, 2, 3},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			rows, cols := calculateGrid(tt.count)
			if rows != tt.wantRows {
				t.Errorf("rows = %d, want %d", rows, tt.wantRows)
			}
			if cols != tt.wantCols {
				t.Errorf("cols = %d, want %d", cols, tt.wantCols)
			}
		})
	}
}

func TestCalculateGridAspect(t *testing.T) {
	// For larger counts, verify the aspect ratio is close to 16:9
	aspect := 16.0 / 9.0

	for count := 10; count <= 50; count++ {
		rows, cols := calculateGrid(count)
		gridAspect := float64(cols) / float64(rows)
		diff := absFloat(gridAspect - aspect)

		// Allow some tolerance
		if diff > 1.0 {
			t.Errorf("count=%d: grid aspect %.2f too far from 16:9 (%.2f), diff=%.2f",
				count, gridAspect, aspect, diff)
		}
	}
}

func TestCalculateGridCoverage(t *testing.T) {
	// Verify that rows * cols >= count (all cameras fit)
	for count := 1; count <= 50; count++ {
		rows, cols := calculateGrid(count)
		capacity := rows * cols
		if capacity < count {
			t.Errorf("count=%d: capacity %d < count", count, capacity)
		}
	}
}

func TestCalculateGridNegative(t *testing.T) {
	rows, cols := calculateGrid(-5)
	if rows != 1 || cols != 1 {
		t.Errorf("calculateGrid(-5) = %dx%d, want 1x1", rows, cols)
	}
}
