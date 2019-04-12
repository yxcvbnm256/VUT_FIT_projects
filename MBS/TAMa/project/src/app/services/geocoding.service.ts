import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';
import { LocationIQGeocoding } from '../models/location-i-q-geocoding';
import { map } from 'rxjs/operators';
import { LocationIQReverseGoecoding } from '../models/location-iqreverse-goecoding';
import {LocationIQAutocompleteItem} from '../models/location-iqautocomplete';

@Injectable()
export class GeocodingService {

  private readonly apiUrlBase = 'https://eu1.locationiq.com/v1/';
  private readonly apiSearch = 'search.php';
  private readonly apiReverse = 'reverse.php';
  private readonly apiAutocomplete = 'autocomplete.php';
  private readonly apiKey = '42dab8996624dd';

  constructor(private http: HttpClient) { }

  getData(query: string): Observable<LocationIQGeocoding> {
    const apiUrl = this.apiUrlBase + this.apiSearch;
    const params = {
      key: this.apiKey,
      q: query,
      format: 'json'
    };
    return this.http.get<LocationIQGeocoding[]>(apiUrl, { params: params }).pipe(
      map(items => items[0]) // TODO: Fix when N/A
    );
  }

  getReversedData(lat: string, lon: string): Observable<LocationIQReverseGoecoding> {
    const apiUrl = this.apiUrlBase + this.apiReverse;
    const params = {
      key: this.apiKey,
      lat: lat,
      lon: lon,
      format: 'json',
      'accept-language': 'cs,en',
    };
    return this.http.get<LocationIQReverseGoecoding>(apiUrl, { params: params });
  }

  autocomplete(query: string): Observable<LocationIQAutocompleteItem[]> {
    const apiUrl = this.apiUrlBase + this.apiAutocomplete;
    const params = {
      key: this.apiKey,
      q: query,
      'accept-language': 'cs,en',
    };
    return this.http.get<LocationIQAutocompleteItem[]>(apiUrl, { params: params });
  }
}
