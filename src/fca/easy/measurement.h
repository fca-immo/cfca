/**
 * easy/measurement.h, (c) 2013, Immanuel Albrecht; Dresden University of
 * Technology, Professur für die Psychologie des Lernen und Lehrens
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FCA__EASY_MEASUREMENT_H_
#define FCA__EASY_MEASUREMENT_H_

void
calculateLikelihood(const FormalContext B, const ConditionMap c,
		const FormalContext I, const EtaFunction eta, CommutativeProduct L);

FormalContext
newFakeMeasurement(const FormalContext I, const EtaFunction eta,
		int experiments, ConditionMap* output_c);

void
optimizeConditionMap(const FormalContext B, ConditionMap c,
		const FormalContext I, const EtaFunction restrict eta,
		const LogCache log_c);

void
optimizeApproximationContext(const FormalContext B, const ConditionMap cmap,
		FormalContext I, const EtaFunction restrict eta, const LogCache log_c);

DistanceMatrix
newDistanceMatrixFromContext(const FormalContext B,
		const EtaFunction restrict eta, const LogCache log_c);

#endif /* MEASUREMENT_H_ */
