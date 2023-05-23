import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { Reservation } from '../entities/reservation';
import { User } from '../entities/user';

@Injectable({
  providedIn: 'root'
})
export class ReservationService {
  private baseUrl="http://localhost:8083";

  constructor(private http: HttpClient) { }

  public addReservation(user: User): Observable<Reservation> {
    return this.http.post<Reservation>(`${this.baseUrl}/home/reserve`, user);
  }
  
}
