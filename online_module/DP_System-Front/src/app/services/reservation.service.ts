import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { Reservation } from '../entities/reservation';

@Injectable({
  providedIn: 'root'
})
export class ReservationService {
  private baseUrl="http://localhost:8083/home";

  constructor(private http: HttpClient) { }

  public getReservations(): Observable<Reservation[]> {
    return this.http.get<Reservation[]>(`${this.baseUrl}/all`);
  }

  public addReservation(reservation: Reservation): Observable<Reservation> {
    return this.http.post<Reservation>(`${this.baseUrl}/add`, reservation);
  }

  public updateReservation(reservation: Reservation): Observable<Reservation> {
    return this.http.put<Reservation>(`${this.baseUrl}/update`, reservation);
  }

  public deleteReservation(reservationId: number): Observable<void> {
    return this.http.delete<void>(`${this.baseUrl}/delete/${reservationId}`);
  }
}
